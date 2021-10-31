#include "includes.h"
#include "SocketUtil.h"

////////////////////////////////////////////////////////////////////////////

void SocketUtil::err_display(int err, std::wostream& wos)
{
	TCHAR* w_msg = nullptr;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr, err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPTSTR>(&w_msg), 0, nullptr);
	MessageBox(nullptr, reinterpret_cast<LPTSTR>(w_msg), nullptr, MB_ICONERROR);
	wos.imbue(std::locale("korean"));
	wos << w_msg << std::endl;
	LocalFree(w_msg);
}

void SocketUtil::terminate()
{
	err_display(WSAGetLastError());
	exit(-1);
}

void SocketUtil::CheckError(int ret_val)
{
	if (SOCKET_SUCCESS != ret_val) { terminate(); }
}

void SocketUtil::CheckError(const SOCKET& socket)
{
	if (SOCKET_SUCCESS == socket) { terminate(); }
}

////////////////////////////////////////////////////////////////////////////

uint32 SocketUtil::caculate_bytes_form_WSABUFs(LPWSABUF buf, uint32 count_of_bufs)
{
	uint32 ret{ 0 };
	for (uint32 i = 0; i < count_of_bufs; i++) {
		ret += buf[i].len;
	};
	return ret;
}

////////////////////////////////////////////////////////////////////////////