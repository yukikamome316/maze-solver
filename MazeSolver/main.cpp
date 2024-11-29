#include <vector>
#include "MazeInterface.h"

int main() {
	//盤面
	std::vector<std::vector<uint32_t>> maze{
		{  9,   1,   1,   3 },
		{  8,  75,   9,   3 },
		{  8,  10,  10,  10 },
		{  8,  12,   6, 190 },
		{ 12,   4,   4,   6 }
	};

	for (int r = 0; r < maze.size(); ++r) {
		for (int c = 0; c < maze[r].size(); ++c) {
			maze[r][c] |= MazeInterface::B_VISITED;
		}
	}

	//画面の幅，画面の高さ，セルの横の数，セルの縦の数
	MazeInterface mint(maze[0].size() * 80, maze.size() * 80, maze[0].size(), maze.size());

	bool isLoop = true;
	//メインループ
	while (isLoop) {
		//裏バッファに描画
		mint.draw(maze);
		//画面に反映 'q'が押されたら終了
		if (!mint.show()) break;
	}

	return 0;
}