#pragma once

#include<opencv2/opencv.hpp>
#include<vector>

class MazeInterface {
public:
	int windowWidth;
	int windowHeight;
	int cellCols;
	int cellRows;
	int cellWidth;
	int cellHeight;
	cv::Mat dispImage;
	cv::Mat cellImage;
	cv::Scalar backgroundColor;
	cv::Scalar wallColor;
	cv::Scalar startColor;
	cv::Scalar goalColor;
	cv::Scalar cartColor;

public:
	MazeInterface(int windowWidth = 400, int windowHeight = 400, int cellCols = 5, int cellRows = 5) :
		windowWidth(windowWidth),
		windowHeight(windowHeight),
		cellCols(cellCols),
		cellRows(cellRows),
		dispImage(windowHeight, windowWidth, CV_8UC3)
	{
		cellWidth = windowWidth / cellCols;
		cellHeight = windowHeight / cellRows;
		cellImage = cv::Mat(cellHeight, cellWidth, CV_8UC3);
		backgroundColor = CV_RGB(255, 255, 255);
		wallColor = CV_RGB(0, 0, 0);
		startColor = CV_RGB(0, 255, 255);
		goalColor = CV_RGB(255, 255, 0);
		cartColor = CV_RGB(255, 0, 0);
	}
	~MazeInterface() {

	}
	/* maze�d�l
	* 2�����z��maze �v�f���FcellRows�~cellCols
	* ��F 5x4
	* 9  1  1   3
	* 8 75  9   3
	* 8 10 10  10
	* 8 12  6 190
	* 12 4  4   6
	*
	* ���͂���Ȋ����̖��H�ł��D
	* ����������������
	* ����������������
	* �����f���@�@�@��
	* �����@���@���@��
	* �����@�@�@���r��
	* ����������������
	* ����������������
	*
	* �e�Z���̐��l�́C���̃Z���ɗאڂ��邷��G�b�W�ɕǂ����邩�������r�b�g��D
	*    8,     7,       6,       5, 4, 3, 2, 1
	* ����,�S�[��,�X�^�[�g,�K��ς�,��,��,�E,��
	* 128,64,32,16,8,4,2,1
	*/

	//�Z���̃r�b�g�t�B�[���h
	enum BCell {
		B_UP = 1 << 0,
		B_RIGHT = 1 << 1,
		B_DOWN = 1 << 2,
		B_LEFT = 1 << 3,
		B_VISITED = 1 << 4,
		B_START = 1 << 5,
		B_GOAL = 1 << 6,
		B_CART = 1 << 7
	};

	//���o�b�t�@�ɕ`�悷�� ���̌�C��ʂɕ\�����邽�߂�show�����s���邱��
	void draw(std::vector<std::vector<uint32_t>>& maze) {
		dispImage.setTo(backgroundColor);

		for (int r = 0; r < cellRows; ++r) {
			for (int c = 0; c < cellCols; ++c) {
				//�w�i�F�ŏ�����
				cellImage.setTo(backgroundColor);
				//
				if (maze[r][c] & B_VISITED) { //�K��m�[�h�Ȃ�
					//����
					//�@��
					int wallWidth = (cellWidth < cellHeight ? cellWidth : cellHeight) / 5;
					if (maze[r][c] & B_UP) {
						cv::rectangle(cellImage, cv::Rect(0, 0, cellWidth, wallWidth), wallColor, -1);
					}
					if (maze[r][c] & B_RIGHT) {
						cv::rectangle(cellImage, cv::Rect(cellWidth - wallWidth, 0, wallWidth, cellHeight), wallColor, -1);
					}
					if (maze[r][c] & B_DOWN) {
						cv::rectangle(cellImage, cv::Rect(0, cellHeight - wallWidth, cellWidth, wallWidth), wallColor, -1);
					}
					if (maze[r][c] & B_LEFT) {
						cv::rectangle(cellImage, cv::Rect(0, 0, wallWidth, cellHeight), wallColor, -1);
					}
					//�A�X�^�[�g�^�S�[��
					int sgRadius = ((cellWidth < cellHeight ? cellWidth : cellHeight) - wallWidth * 2) * 0.8 / 2.0;
					if (maze[r][c] & B_GOAL) {
						cv::circle(cellImage, cv::Point(cellWidth / 2, cellHeight / 2), sgRadius, goalColor, -1);
					}
					if (maze[r][c] & B_START) {
						cv::circle(cellImage, cv::Point(cellWidth / 2, cellHeight / 2), sgRadius, startColor, -1);
					}
					//�B�ԑ�
					int cartRadius = ((cellWidth < cellHeight ? cellWidth : cellHeight) - wallWidth * 2) * 0.6 / 2.0;
					if (maze[r][c] & B_CART) {
						cv::circle(cellImage, cv::Point(cellWidth / 2, cellHeight / 2), cartRadius, cartColor, -1);
					}
				}
				else { //�K��m�[�h�łȂ��Ȃ�
					//�`���Ȃ�
				}
				//���f
				cv::Mat part(dispImage, cv::Rect(cellWidth * c, cellHeight * r, cellWidth, cellHeight));
				cellImage.copyTo(part);
				cv::imwrite("tmp.png", dispImage);
			}
		}
	}

	//���o�b�t�@�ɕ`�悳�ꂽ���H����ʂɕ\������
	//q�������ꂽ��false, �����łȂ����true��Ԃ�
	bool show() {
		cv::imshow("maze", dispImage);
		return cv::waitKey(1) != 'q';
	}
};
