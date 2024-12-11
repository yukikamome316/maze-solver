//
// �V���A���ʐM�̃T���v���v���O�����@���M�Ǝ�M
//


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <wchar.h>
#include <math.h>
#include "qedit.h"

#define BUFSIZE 1024

bool Ret;
OVERLAPPED readOverlapped;
OVERLAPPED writeOverlapped;


//�����ݒ�
bool setupSerial(HANDLE &serialPort, LPCWSTR com, int baudrate) {

	//�V���A���ʐM�p�|�[�g�̊m��
	serialPort = CreateFile(com, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (serialPort == INVALID_HANDLE_VALUE) {
		OutputDebugString(L"PORT COULD NOT OPEN");
		return false;
	}
	Ret = SetupComm(serialPort, 1024, 1024);
	if (!Ret) {
		OutputDebugString(L"SET UP FAILED");
		CloseHandle(serialPort);
		return false;
	}
	Ret = PurgeComm(serialPort, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!Ret) {
		OutputDebugString(L"CLEAR FAILED");
		CloseHandle(serialPort);
		return false;
	}

	//��{�ʐM�����̐ݒ�
	DCB dcb;
	GetCommState(serialPort, &dcb);
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = baudrate;
	dcb.fBinary = TRUE;
	dcb.ByteSize = 8;
	dcb.fParity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	Ret = SetCommState(serialPort, &dcb);
	if (!Ret) {
		OutputDebugString(L"SetCommState FAILED");
		CloseHandle(serialPort);
		return false;
	}

	return true;
}

//���M
bool sendSerial(HANDLE serialPort, const char* data) {
	DWORD dwSendSize;
	Ret = WriteFile(serialPort, data, strlen(data), &dwSendSize, NULL);
	if (!Ret) {
		DWORD err = GetLastError();
		if (err == ERROR_IO_PENDING) {
			DWORD num;
			GetOverlappedResult(serialPort, &writeOverlapped, &num, TRUE);
		}
		else {
			OutputDebugString(L"SEND FAILED\n");
			CloseHandle(serialPort);
			return false;
		}
	}
	return true;
}

//��M
bool recvSerial(HANDLE serialPort, char* rbuf) {

	//�V���A����M	
	DWORD dwErrorMask;
	DWORD dwRecvSize;
	DWORD dwCount;
	COMSTAT comStat;

	ClearCommError(serialPort, &dwErrorMask, &comStat);
	dwCount = comStat.cbInQue;
	if (dwCount > 0) {
		memset(rbuf, 0, sizeof(rbuf));
		Ret = ReadFile(serialPort, rbuf, dwCount, &dwRecvSize, &readOverlapped);

		DWORD err = GetLastError();
		if (FAILED(err)) {
			return false;
		}
		else if (err == ERROR_IO_PENDING) {
			GetOverlappedResult(serialPort, &readOverlapped, &dwRecvSize, TRUE);
		}
		else if (!Ret) {
			printf("RECV FAILED\n");
			system("PAUSE");
			CloseHandle(serialPort);
			return false;
		}
	}

	//�I�[������K������(�t�Ƃ�����Ȃ��悤��)
	rbuf[dwCount] = '\0';
	return true;
}

//�G���[����
void printError(DWORD err) {
	LPSTR lpMsgBuf = nullptr;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // ����̌���
		(LPSTR)&lpMsgBuf,
		0,
		NULL
	);

	std::cout << "[" << std::hex << err << "] " << lpMsgBuf << std::endl;

	LocalFree(lpMsgBuf);
}