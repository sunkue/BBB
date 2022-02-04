#pragma once

#include "IocpHelper.h"


// => atomic enum으로 세션의 동시성을 지킨다.
enum SESSION_STATE : int8
{
	ST_FREE,

	ST_ON_DICONNECT,

	ST_ON_ACCEPT,

	ST_IO_ABLE_LINE = 100,

	ST_ACCEPT,

	ST_INGAME
};

class Session
{
	friend class IOCP;
public:
	Session() = default;
	~Session() { do_disconnect(); }

public:
	void NewSession(SOCKET socket, NetID net_id);

private:
	void clear(SESSION_STATE state = SESSION_STATE::ST_FREE);
	bool HandleIoError(int res);

public:
	void do_recv();
	void do_send(const void* packet, size_t packet_len);
	void do_disconnect();

public:
	GET(net_id);
	GET(prerecv_size);
	GET_REF(recv_over);
	GET_REF_UNSAFE(recv_over);

	SET(prerecv_size);
	SET(state);
	
public:
	bool check_state(const SESSION_STATE state)const { return  state == state_; }
	bool check_state_good()const /* IO able */ { return ST_IO_ABLE_LINE < state_; }
	bool check_state_bad()const { return !check_state_good(); }

private:
	ExpOver recv_over_{ COMP_OP::OP_RECV };
	DWORD	prerecv_size_{ 0 };
	SOCKET	socket_{ INVALID_SOCKET };
	SESSION_STATE state_{ SESSION_STATE::ST_FREE }; 

	NetID	net_id_{ 0 };
};