#include <conio.h>  // Include the conio.h header for keyboard input

#include <iostream>
#include <queue>
#include <string>
#include <vector>

#include "MazeInterface.h"
#include "color.h"
#include "serial.h"

using namespace std;

enum class Direction { UP, DOWN, LEFT, RIGHT, NONE };

enum class CarMode { TRACKING, ROTAION };

pair<int, int> findStart(const vector<vector<uint32_t>>& maze) {
  for (int r = 0; r < maze.size(); ++r) {
    for (int c = 0; c < maze[r].size(); ++c) {
      if (maze[r][c] & MazeInterface::B_START) {
        return {r, c};
      }
    }
  }
  return {-1, -1};
}

pair<int, int> findGoal(const vector<vector<uint32_t>>& maze) {
  for (int r = 0; r < maze.size(); ++r) {
    for (int c = 0; c < maze[r].size(); ++c) {
      if (maze[r][c] & MazeInterface::B_GOAL) {
        return {r, c};
      }
    }
  }
  return {-1, -1};
}

void bfs(const vector<vector<uint32_t>>& maze, int startRow, int startCol,
         vector<vector<int>>& dist, vector<vector<pair<int, int>>>& prev) {
  queue<pair<int, int>> q;
  q.push({startRow, startCol});
  dist[startRow][startCol] = 0;

  const vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  const vector<uint32_t> barriers = {MazeInterface::B_UP, MazeInterface::B_DOWN,
                                     MazeInterface::B_LEFT,
                                     MazeInterface::B_RIGHT};

  while (!q.empty()) {
    int row = q.front().first;
    int col = q.front().second;
    q.pop();

    for (int i = 0; i < directions.size(); ++i) {
      int newRow = row + directions[i].first;
      int newCol = col + directions[i].second;
      if (newRow >= 0 && newRow < maze.size() && newCol >= 0 &&
          newCol < maze[0].size() && !(maze[row][col] & barriers[i]) &&
          dist[newRow][newCol] == -1) {
        q.push({newRow, newCol});
        dist[newRow][newCol] = dist[row][col] + 1;
        prev[newRow][newCol] = {row, col};
      }
    }
  }
}

void markShortestPath(vector<vector<uint32_t>>& maze,
                      const vector<vector<pair<int, int>>>& prev, int goalRow,
                      int goalCol) {
  int row = goalRow;
  int col = goalCol;
  while (row != -1 && col != -1) {
    maze[row][col] |= MazeInterface::B_PATH;
    int prevRow = prev[row][col].first;
    int prevCol = prev[row][col].second;
    row = prevRow;
    col = prevCol;
  }
}

void moveCar(vector<vector<uint32_t>>& maze, int& nx, int& ny, Direction dir) {
  const vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  const vector<uint32_t> barriers = {MazeInterface::B_UP, MazeInterface::B_DOWN,
                                     MazeInterface::B_LEFT,
                                     MazeInterface::B_RIGHT};

  int newX = nx + directions[static_cast<int>(dir)].second;
  int newY = ny + directions[static_cast<int>(dir)].first;

  if (newY >= 0 && newY < maze.size() && newX >= 0 && newX < maze[0].size() &&
      !(maze[ny][nx] & barriers[static_cast<int>(dir)])) {
    maze[ny][nx] &= ~MazeInterface::B_CAR;
    nx = newX;
    ny = newY;
    maze[ny][nx] |= MazeInterface::B_CAR;
  }
}

pair<int, int> findCar(const vector<vector<uint32_t>>& maze) {
  for (int r = 0; r < maze.size(); ++r) {
    for (int c = 0; c < maze[r].size(); ++c) {
      if (maze[r][c] & MazeInterface::B_CAR) {
        return {c, r};
      }
    }
  }
  return {-1, -1};
}

Direction getDirection(const vector<vector<uint32_t>>& maze, int nx, int ny) {
  const vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  const vector<uint32_t> barriers = {MazeInterface::B_UP, MazeInterface::B_DOWN,
                                     MazeInterface::B_LEFT,
                                     MazeInterface::B_RIGHT};

  for (int i = 0; i < directions.size(); ++i) {
    int newX = nx + directions[i].second;
    int newY = ny + directions[i].first;
    if (newY >= 0 && newY < maze.size() && newX >= 0 && newX < maze[0].size() &&
        !(maze[ny][nx] & barriers[i]) &&
        (maze[newY][newX] & MazeInterface::B_PATH) &&
        !(maze[newY][newX] & MazeInterface::B_VISITED)) {
      return static_cast<Direction>(i);
    }
  }
  return Direction::NONE;
}

void handleSerialData(HANDLE sp, vector<vector<uint32_t>>& maze, int& nx,
                      int& ny, Direction& ndir, CarMode& mode) {
  std::string completeRecvStr;

  bool received = false;
  while (!received) {
    char rbuf[BUFSIZE];
    if (recvSerial(sp, rbuf) == NULL) {
      std::cout << "Error on recvSerial" << std::endl;
      printError(GetLastError());
      system("PAUSE");
      exit(0);
    }
    completeRecvStr += rbuf;

    size_t newlinePos = completeRecvStr.find('\n');
    if (newlinePos != std::string::npos) {
      received = true;

      Color color;
      std::string dataToParse = completeRecvStr.substr(0, newlinePos);
      if (sscanf(dataToParse.c_str(), "%u,%u,%u", &color.red, &color.green,
                 &color.blue) != 3) {
        // std::cerr << "Error parsing data" << std::endl;
        // std::cerr << "Data: " << dataToParse << std::endl;
        return;
      }

      if (redRange.isInRange(color)) {
        // red detected

        std::cout << "Red detected!!!" << std::endl;
        // color.print();

        if (mode == CarMode::TRACKING) {
          mode = CarMode::ROTAION;
        } else if (mode == CarMode::ROTAION) {
          // 回転後、交差点を出るまでは回転をしない
          return;
        }

        moveCar(maze, nx, ny, ndir);
        maze[ny][nx] |= MazeInterface::B_VISITED;

        Direction newDir = getDirection(maze, nx, ny);

        std::string rotationStr;

        if (newDir == ndir) {
          cout << "same direction (go forward)" << endl;
          rotationStr = "F";
        } else if (newDir == Direction::NONE) {
          // std::cout << "ゴールに到達しました" << endl;
          rotationStr = "G";
        } else if (ndir == Direction::UP) {
          if (newDir == Direction::RIGHT) {
            rotationStr = "R";
          } else if (newDir == Direction::LEFT) {
            rotationStr = "L";
          }
        } else if (ndir == Direction::DOWN) {
          if (newDir == Direction::RIGHT) {
            rotationStr = "L";
          } else if (newDir == Direction::LEFT) {
            rotationStr = "R";
          }
        } else if (ndir == Direction::LEFT) {
          if (newDir == Direction::UP) {
            rotationStr = "R";
          } else if (newDir == Direction::DOWN) {
            rotationStr = "L";
          }
        } else if (ndir == Direction::RIGHT) {
          if (newDir == Direction::UP) {
            rotationStr = "L";
          } else if (newDir == Direction::DOWN) {
            rotationStr = "R";
          }
        }

        ndir = newDir;
        cout << "change direction: " << rotationStr << endl;

        if (!sendSerial(sp, rotationStr.c_str())) {
          puts("Error on sendSerial");
          printError(GetLastError());
          system("PAUSE");
          exit(0);
        }
      } else if (blackRange.isInRange(color)) {
        // black detected

        std::cout << "Black detected!!!" << std::endl;

        if (mode == CarMode::ROTAION) {
          mode = CarMode::TRACKING;
        }
      }
    }
  }
}

int main() {
  HANDLE sp;
  LPCWSTR com = L"COM3";
  int baudrate = 9600;

  if (!setupSerial(sp, com, baudrate)) {
    std::cout << "Error on setupSerial" << std::endl;
    printError(GetLastError());
    system("PAUSE");
    exit(0);
  }

  vector<vector<uint32_t>> maze{
      {77, 5, 5, 1}, {9, 1, 5, 4}, {10, 10, 9, 1}, {14, 12, 6, 172}};

  auto [startRow, startCol] = findStart(maze);
  auto [goalRow, goalCol] = findGoal(maze);

  vector<vector<int>> dist(maze.size(), vector<int>(maze[0].size(), -1));
  vector<vector<pair<int, int>>> prev(
      maze.size(), vector<pair<int, int>>(maze[0].size(), {-1, -1}));
  bfs(maze, startRow, startCol, dist, prev);
  markShortestPath(maze, prev, goalRow, goalCol);

  MazeInterface mint(maze[0].size() * 80, maze.size() * 80, maze[0].size(),
                     maze.size());

  bool isLoop = true;
  bool initialized = false;
  while (isLoop) {
    auto [nx, ny] = findCar(maze);
    Direction ndir;
    CarMode mode;

    if (!initialized) {
      ndir = getDirection(maze, nx, ny);
      mode = CarMode::TRACKING;

      std::cout << "Initial direction: ";
      switch (ndir) {
        case Direction::UP:
          std::cout << "UP" << endl;
          break;
        case Direction::DOWN:
          std::cout << "DOWN" << endl;
          break;
        case Direction::LEFT:
          std::cout << "LEFT" << endl;
          break;
        case Direction::RIGHT:
          std::cout << "RIGHT" << endl;
          break;
        default:
          break;
      }
      initialized = true;
    }

    if (nx == -1 || ny == -1) {
      cerr << "スマートカーが見つかりませんでした" << endl;
      break;
    }

    maze[ny][nx] |= MazeInterface::B_VISITED;

    handleSerialData(sp, maze, nx, ny, ndir, mode);

    if (ndir == Direction::NONE) {
      // wcout << L"ゴールに到達しました" << endl;
      // break;
    }

    mint.draw(maze);
    if (!mint.show()) break;
  }

  CloseHandle(sp);
  return 0;
}
