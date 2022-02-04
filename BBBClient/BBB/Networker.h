#pragma once
class Networker
{
	SINGLE_TON(Networker);

	~Networker();

public:
	void connect(const char* server_ip);

	void do_recv();

	void do_send(const void* packet, size_t packet_len);

	void process_packet(const char* const packet);

private:
	SOCKET socket_;
	array<char, MAX_PACKET_SIZE + 1> recv_buf_{};
};

