#include "stdafx.h"
#include "Session.h"
#include "IOCP.h"

void InitWsa()
{
	WSADATA WSAData;
	int res = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (SOCKET_ERROR == res)
	{
		SocketUtil::ReportError("InitWsa FAIL.");
		exit(-1);
	}
}

void EndWsa()
{
	WSACleanup();
}


int main()
{
	InitWsa();

	thread repeatsend{ &IOCP::RepeatSendLoop, &IOCP::get(), 200ms };

	IOCP::get().ProcessQueuedCompleteOperationLoop();

	repeatsend.join();
	;

	EndWsa();
}