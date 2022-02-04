#pragma once

#include "stdafx.h"

constexpr int MAX_BUFFER_SIZE = MAX_PACKET_SIZE;
static_assert(MAX_PACKET_SIZE <= MAX_BUFFER_SIZE);

enum class COMP_OP : int8
{
	OP_NONE,
	OP_RECV,
	OP_SEND,
	OP_ACCEPT,
	OP_DISCONNECT
};

struct ExpOver
{
	WSAOVERLAPPED	wsa_over{};
	COMP_OP			comp_op{ COMP_OP::OP_NONE };
	char			net_buf[MAX_BUFFER_SIZE]{};
	WSABUF			wsa_buf{ sizeof(net_buf), net_buf };

	ExpOver(COMP_OP op, size_t packet_len, const void* packet) : comp_op(op)
	{
		wsa_buf.len = static_cast<ULONG>(packet_len);
		memcpy(net_buf, packet, packet_len);
	}

	ExpOver(COMP_OP op) : comp_op(op) {}

	void clear()
	{
		ZeroMemory(&wsa_over, sizeof(wsa_over));
		ZeroMemory(net_buf, sizeof(net_buf));
		wsa_buf.len = sizeof(net_buf);
		wsa_buf.buf = net_buf;
	}
};
