#include <vector>
#include "MazeInterface.h"

int main() {
	//�Ֆ�
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

	//��ʂ̕��C��ʂ̍����C�Z���̉��̐��C�Z���̏c�̐�
	MazeInterface mint(maze[0].size() * 80, maze.size() * 80, maze[0].size(), maze.size());

	bool isLoop = true;
	//���C�����[�v
	while (isLoop) {
		//���o�b�t�@�ɕ`��
		mint.draw(maze);
		//��ʂɔ��f 'q'�������ꂽ��I��
		if (!mint.show()) break;
	}

	return 0;
}