//
// シリアル通信のサンプルプログラム　送信と受信
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


//初期設定
bool setupSerial(HANDLE &serialPort, LPCWSTR com, int baudrate) {

	//シリアル通信用ポートの確立
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

	//基本通信条件の設定
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

//送信
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

//受信
bool recvSerial(HANDLE serialPort, char* rbuf) {

	//シリアル受信	
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

	//終端文字を必ずつける(フとか言わないように)
	rbuf[dwCount] = '\0';
	return true;
}

//エラー処理
void printError(DWORD err) {
	LPSTR lpMsgBuf = nullptr;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 既定の言語
		(LPSTR)&lpMsgBuf,
		0,
		NULL
	);

	std::cout << "[" << std::hex << err << "] " << lpMsgBuf << std::endl;

	LocalFree(lpMsgBuf);
}