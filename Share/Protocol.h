#pragma once


#include "types.h"
#include "enum.h"

//====================================

const uint16_t SERVER_PORT = 8282;
const auto SERVER_IP = "127.0.0.1";

//====================================

const int MAX_NAME_SIZE = 10;
const int MAX_PLAYER = 20;
const int MAX_ENEMY = 30;
const int MAX_OBJECT = MAX_PLAYER + MAX_ENEMY;
const int MAX_PACKET_SIZE = 256;

//====================================

#pragma warning(push)
#pragma warning(disable: 26812)

BETTER_ENUM
(
	PAKCET_TYPE, int8

	, NONE = 0
	
	/* Client 2 Server */
	
	, CS_NONE			= 10
	, CS_TRY_LOGIN
	, CS_LOGIN_PACKET
	, CS_MAKE_MAP_DONE
	, CS_NEW_CHARATOR




	/* Server 2 Client */

	, SC_NONE			= 100





);

#pragma warning(pop)


#pragma pack (push, 1)
//================ BASE ================


template<class T>
struct packet_base
{
	int8 size = sizeof(T);
	PAKCET_TYPE packet_type = +PAKCET_TYPE::_from_string_nocase(typeid(T).name() + 7);
};
#define PACKET(name) struct name : packet_base<name>											

PACKET(none)
{
};

//=============== LOG_IN =================

// => �� �̸����� �α��� �ҷ�
PACKET(cs_try_login)
{
	char name[MAX_NAME_SIZE];
};


// => ok ���� id�� �̰ž�

PACKET(sc_ok_login)
{
	int8 login_id;
};

// �ʿ� ���������� ����� �ִ��� �˷��־�� ��
// PACKET(sc_new_charator) * numofchractor; to new player

PACKET(sc_new_charator)
{
	int8 login_id;
	char name[MAX_NAME_SIZE];
	float x;
	float y;
};

PACKET(cs_make_map_done)
{

};

// �ٸ� Ŭ���̾�Ʈ�� ���� �� ĳ���� �α��ξ˸�
// PACKET(sc_new_charator); to old players

//============= LOG_OUT ===========

// => logout (esc ����)

PACKET(sc_logout)
{
	int8 logout_id;
};

// => recv �� ���ϰ��� 0 �� ��� �Ǵ� PACKET(sc_logout) ��Ŷ�� ���� ���.

PACKET(cs_logout)
{
	int8 logout_id;
};

// => �ʿ��� �÷��̾� ����.

//=============== MOVE_INPUT =================

// �̵������ڵ�, ��Ʈ�������� press,unpress ����
enum class MOVE_DIR : int8
{
	FORWARD = 1 << 0,
	BACK = 1 << 1,
	LEFT = 1 << 2,
	RIGHT = 1 << 3
};

// => input �� ��ȭ��

PACKET(cs_moved)
{
	MOVE_DIR arrow_key;
};

// => �ٸ��÷��̾������ ����

PACKET(sc_moved)
{
	int8 mover_id;
	MOVE_DIR arrow_key;
};

//=============== CHATTING =================

// => ä�� enter

PACKET(cs_chat)
{
	char chat[30];
	char padding = '\0';
};

// => �ٸ� �÷��̾������ ����

PACKET(sc_chat)
{
	int8 chatter_id;
	char chat[30];
	char padding = '\0';
};

//============= Ÿ�� ===========

// => �÷��̾��� Ÿ�ݽõ�

PACKET(sc_try_attack)
{
	int8 defender_id;
};

// => �������� ��ġ�� ���ӿ��θ� ���� ����

PACKET(cs_attacked)
{
	int8 attacker_id;
	int8 defender_id;
};

// => attacket �� ���� ����Ʈ �ߵ�
// => defender �� �ǰ� ����Ʈ �ߵ�



//==================== MODEL_CHANGE ==========================

// => �����ҷ�

PACKET(cs_model_change)
{
	int8 model_id;
};

// => ok, ��� �÷��̾������ ����

PACKET(sc_model_change)
{
	int8 id;
	int8 model_id;
};

// => ����.

//================================================


#pragma pack(pop)
