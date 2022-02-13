#pragma once


#include "types.h"
#include "enum.h"
//====================================

using NetID = uint8;
using packet_size_t = uint8;

//====================================

const uint16_t SERVER_PORT = 8282;

//====================================

const int MAX_NAME_SIZE = 10;
const int MAX_PLAYER = 8;
const int MAX_ENEMY = 30;
const int MAX_OBJECT = MAX_PLAYER + MAX_ENEMY;
const int MAX_PACKET_SIZE = std::numeric_limits<packet_size_t>::max() + 1;
const int MAX_BUFFER_SIZE = MAX_PACKET_SIZE + 1;
static_assert(MAX_PACKET_SIZE <= MAX_BUFFER_SIZE);

//====================================

constexpr int MAX_HP = 7;
constexpr float DEFAULT_X = 840.f;
constexpr float DEFAULT_Y = 639.5f;

//====================================

#pragma warning(push)
#pragma warning(disable: 26812)

BETTER_ENUM
(
	PACKET_TYPE, int8

	, NONE = 0

	/* Client 2 Server */

	, CS_NONE = 10
	, CS_HI
	, CS_INFO

	/* Server 2 Client */

	, SC_NONE = 100
	, SC_HI_OK
	, SC_NEW_CHARACTOR
	, SC_INFO
	, SC_TEST_HEART_BIT
);

#pragma warning(pop)


#pragma pack (push, 1)
//================ BASE ================


template<class T>
struct packet_base
{
	packet_size_t size = sizeof(T);
	PACKET_TYPE packet_type = +PACKET_TYPE::_from_string_nocase(typeid(T).name() + 7);
};
#define PACKET(name) struct name : packet_base<name>											

PACKET(none)
{
};

//=============== new connection =================

PACKET(cs_hi)
{
};

PACKET(sc_hi_ok)
{
	NetID your_netid;
};

PACKET(sc_new_charactor)
{
	NetID netid;
	float x;
	float z;
	float headx;
	float headz;
};

//=============== info =================

PACKET(cs_info)
{
	float x;
	float z;
	float headx;
	float headz;
};

PACKET(sc_info)
{
	NetID netid;
	float x;
	float z;
	float headx;
	float headz;
};

//=============== reapeat =================

PACKET(sc_test_heart_bit)
{
	std::chrono::milliseconds time_after_send;
};








#pragma pack(pop)
