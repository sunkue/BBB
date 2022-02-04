#include "stdafx.h"
#include "Game.h"
#include "Networker.h"

/// ///////////////////////

Networker::Networker()
{
	WSADATA WSAData;
	int res = WSAStartup(MAKEWORD(2, 3), &WSAData);
	if (SOCKET_ERROR == res)
	{
		SocketUtil::ReportError("InitWsa FAIL.");
		exit(-1);
	}

	socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

}

Networker::~Networker()
{
	::closesocket(socket_);
	WSACleanup();
}

/// ///////////////////////

void Networker::connect(const char* server_ip)
{
	SOCKADDR_IN server_addr; ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = ::htons(SERVER_PORT);
	auto ret = ::inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
	char addr[30];
	::inet_ntop(AF_INET, &server_addr.sin_addr, addr, sizeof(addr));
	cout << addr << endl;
	if (1 != ret)
	{
		SocketUtil::ReportError("inet_pton fail.");
		SocketUtil::DisplayError(WSAGetLastError());
		exit(-1);
	}

	//	char tcp_opt = 1;
	//	::setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (char*)&tcp_opt, sizeof(tcp_opt));

	ret = ::connect(socket_, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

	if (SOCKET_ERROR == ret)
	{
		SocketUtil::ReportError("connect fail, maybe 서버 안 연 듯.");
		SocketUtil::DisplayError(WSAGetLastError());
		exit(-1);
	}
}

/// ///////////////////////
extern bool g_alive;

void Networker::do_recv()
{
	cs_hi hi_packet; do_send(&hi_packet, sizeof(hi_packet));

	auto prerecved = 0;
	while (!glfwWindowShouldClose(Game::get().window))
	{
		auto recved_bytes = recv(socket_, recv_buf_.data() + prerecved, MAX_PACKET_SIZE - prerecved, NULL);
		if (SOCKET_ERROR == recved_bytes)
		{
			SocketUtil::DisplayError(WSAGetLastError());
		}

		if (0 == recved_bytes)
		{
			cout << "disconnected server" << endl;
			exit(-1);
		}

		auto pck_start = recv_buf_.data();
		auto remain_bytes = recved_bytes + prerecved;

		// process completed packets.
		for (auto need_bytes = *reinterpret_cast<packet_size_t*>(pck_start);
			need_bytes <= remain_bytes;)
		{
			process_packet(pck_start);
			pck_start += need_bytes;
			remain_bytes -= need_bytes;
			need_bytes = *reinterpret_cast<packet_size_t*>(pck_start);

			if (0 == remain_bytes || 0 == need_bytes)
			{
				prerecved = 0;
				break;
			}
		}

		// remain_bytes가 남아있으면 미완성패킷의 데이터임. prerecv 해주기.
		if (0 != remain_bytes)
		{
			prerecved = remain_bytes;
			memmove(recv_buf_.data(), pck_start, remain_bytes);
		}
	}
}

/// ///////////////////////

void Networker::do_send(const void* packet, size_t packet_len)
{
	auto ret = ::send(socket_, reinterpret_cast<const char*>(packet), packet_len, NULL);

	if (SOCKET_ERROR == ret)
	{
		SocketUtil::ReportError("send error,,??");
		SocketUtil::DisplayError(WSAGetLastError());
		exit(-1);
	}
}

/// ///////////////////////

void Networker::process_packet(const char* const packet)
{
	const packet_base<void>* pck_base = reinterpret_cast<const packet_base<void>*>(packet);
	packet_size_t pck_size = pck_base->size;
	PACKET_TYPE pck_type = pck_base->packet_type;

	switch (pck_type)
	{
	case PACKET_TYPE::NONE:
	{

	}
	CASE PACKET_TYPE::SC_HI_OK :
	{
		auto pck = reinterpret_cast<const sc_hi_ok*>(packet);
	
		while (Renderer::get().get_cars().size() < (int)pck->your_netid)
			;;;
		
		auto& car = Renderer::get().get_cars().at((int)pck->your_netid);

		Renderer::get().swap_player_ghost();
		Renderer::get().set_ghost(car);
		Renderer::get().swap_player_ghost();
	}
	CASE PACKET_TYPE::SC_INFO :
	{
		auto pck = reinterpret_cast<const sc_info*>(packet);
		auto& car = Renderer::get().get_cars().at((int)pck->netid);
		auto pos = car->get_position();
		pos.x -= pck->x; pos.z -= pck->z;
		car->move(-pos);
	}
	CASE PACKET_TYPE::SC_TEST_HEART_BIT :
	{
		auto pck = reinterpret_cast<const sc_test_heart_bit*>(packet);

		cs_info p;
		auto& player = Renderer::get().get_player();
		p.x = player->get_position().x;
		p.z = player->get_position().z;
		do_send(&p, p.size);
	}
	break; default: break;
	}
}


/// ///////////////////////
