#include "stdafx.h"
#include "IocpHelper.h"
#include "ListenSocket.h"
#include "Session.h"
#include "IOCP.h"

// =============================================== // 

void IOCP::process_packet(const NetID net_id, const void* const packet)
{
	auto SendtoOthers = [&](const auto& sendto_others_packet)
	{
		for (auto& s : sessions_)
		{
			if (s.check_state_bad())
			{
				continue;
			}

			if (s.get_net_id() == net_id)
			{
				continue;
			}

			s.do_send(&sendto_others_packet, sizeof(sendto_others_packet));
		}
	};

	auto SendtoMeOthers = [&](const auto& sendto_me_packet, const auto& sendto_others_packet)
	{
		for (auto& s : sessions_)
		{
			if (s.check_state_bad())
			{
				continue;
			}

			if (s.get_net_id() == net_id)
			{
				s.do_send(&sendto_me_packet, sizeof(sendto_me_packet));
			}
			else
			{
				s.do_send(&sendto_others_packet, sizeof(sendto_others_packet));
			}
		}
	};

	/////

	auto packet_type = reinterpret_cast<const packet_base<void>*>(packet)->packet_type;
	switch (packet_type)
	{
	case PACKET_TYPE::CS_HI:
	{
		cout << (int)net_id << "::hi" << endl;
		sc_hi_ok p;
		p.your_netid = net_id;
		sessions_[net_id].do_send(&p, p.size);
	}
	CASE PACKET_TYPE::CS_INFO:
	{
		auto pck = reinterpret_cast<const cs_info*>(packet);
		sc_info p;
		p.netid = net_id;
		p.x = pck->x;
		p.z = pck->z;
		p.headx = pck->headx;
		p.headz = pck->headz;
		SendtoOthers(p);
	}
	break; default: break;
	}
}

// =============================================== // 

IOCP::IOCP()
{
	iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	ListenSocket::get().Init(iocp_);
	ListenSocket::get().do_accept();
}

// =============================================== // 

void IOCP::ProcessQueuedCompleteOperationLoop()
{
	while (true)
	{
		DWORD returned_bytes;
		NetID net_id;
		WSAOVERLAPPED* p_over;
		BOOL res = GetQueuedCompletionStatus(iocp_, &returned_bytes, (PULONG_PTR)&net_id, &p_over, INFINITE);
		auto ex_over = reinterpret_cast<ExpOver*>(p_over);
		// cerr << "GQCS returned. net_id::" << (int)net_id << "::" << (int)ex_over->comp_op << endl;

		if (FALSE == res) [[unlikely]]
		{
			cerr << "################GQCS::missconnect::" << WSAGetLastError() << "::" << endl;
			if (0 == returned_bytes)
			{
				cerr << "$$$$$$$$$$$$$$$$$$$$$$GQCS::missconnect::" << WSAGetLastError() << "::" << endl;
				sessions_[net_id].do_disconnect();
				continue;
			}
			continue;
		};

		switch (ex_over->comp_op)
		{
		case COMP_OP::OP_RECV: OnRecvComplete(net_id, returned_bytes, ex_over); break;
		case COMP_OP::OP_SEND: OnSendComplete(net_id, returned_bytes, ex_over); break;
		case COMP_OP::OP_ACCEPT: OnAcceptComplete(ex_over); break;
		case COMP_OP::OP_DISCONNECT: OnDisconnectComplete(static_cast<SOCKET>(returned_bytes), net_id, ex_over); break;
		default: cerr << "couldn't be here!! COMPO_OP ERROR ::" << (int)ex_over->comp_op << "::" << endl;
		}
	}
}

// =============================================== // 

void IOCP::OnRecvComplete(NetID net_id, DWORD returned_bytes, ExpOver* ex_over)
{
	//cerr << "RECV " << endl;

	Session& client = sessions_[net_id];

	if (0 == returned_bytes) [[unlikely]]
	{
		client.do_disconnect();
		return;
	};

	auto pck_start = ex_over->net_buf;
	auto remain_bytes = returned_bytes + client.get_prerecv_size();

	// process completed packets.
	for (auto need_bytes = *reinterpret_cast<packet_size_t*>(pck_start);
		need_bytes <= remain_bytes;)
	{
		process_packet(net_id, pck_start);

		pck_start += need_bytes;
		remain_bytes -= need_bytes;
		need_bytes = *reinterpret_cast<packet_size_t*>(pck_start);

		if (0 == remain_bytes)
		{
			break;
		}
	}

	client.set_prerecv_size(remain_bytes);

	// remain_bytes가 남아있으면 미완성패킷의 데이터임. prerecv 해주기.
	if (0 != remain_bytes)
	{
		memmove(&ex_over->net_buf, pck_start, remain_bytes);
	}

	if (client.check_state_good())
		client.do_recv();
}


void IOCP::OnSendComplete(NetID net_id, DWORD returned_bytes, ExpOver* ex_over)
{
	//cerr << "SEND " << endl;

	if (returned_bytes != ex_over->wsa_buf.len)
	{
		Session& client = sessions_[net_id];
		client.do_disconnect();
	}

	delete ex_over;
}

// =============================================== // 


void IOCP::OnAcceptComplete(ExpOver* ex_over) // 유일성 보장 함수.
{
	cerr << "ACCEPT " << endl;
	SOCKET new_socket = *(reinterpret_cast<SOCKET*>(ex_over->net_buf));
	NetID new_id = get_new_net_id();
	Session& new_client = sessions_[new_id];
	{
		new_client.clear(ST_ON_ACCEPT);
		new_client.NewSession(new_socket, new_id);

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(new_socket), iocp_, new_id, 0);
		cerr << "new id::" << (int)new_id << ". accept. " << endl;
		new_client.set_state(ST_ACCEPT);
	}
	new_client.do_recv();
	ListenSocket::get().do_accept();
}

NetID IOCP::get_new_net_id()
{
	for (int net_id = 0; net_id < sessions_.size(); net_id++)
	{
		if (sessions_[net_id].check_state(ST_FREE))
		{
			sessions_[net_id].set_state(ST_ON_ACCEPT);
			return net_id;
		}
	}
	cerr << "SESSION_FULL!!!!!" << sessions_.size() << ":" << MAX_PLAYER << endl;
	return 0;
}

// =============================================== // 

void IOCP::OnDisconnectComplete(SOCKET s, NetID net_id, ExpOver* ex_over)
{
	cout << "diconnectd!!!" << (int)net_id << "::socket" << s << "::" << endl;

	auto res = closesocket(s);
	if (SOCKET_ERROR == res)
	{
		cout << "closeerr::" << WSAGetLastError() << "::" << endl;
	}
	sessions_[net_id].clear();
}

// =============================================== // 

void IOCP::RepeatSendLoop(milliseconds repeat_time)
{
	while (true)
	{
		auto curr = std::chrono::high_resolution_clock::now();
		static auto TimeAfterSend = 0ms;
		static auto past = curr;

		TimeAfterSend += duration_cast<milliseconds>(curr - past);

		if (TimeAfterSend < repeat_time)
		{
			continue;
		}

		for (auto& s : sessions_)
		{
			if (s.check_state_good())
			{
				sc_test_heart_bit x;
				x.time_after_send = TimeAfterSend;
				s.do_send(&x, sizeof(sc_test_heart_bit));
			}
		}

		past = curr;
		TimeAfterSend = 0ms;
	}
}

// =============================================== // 
