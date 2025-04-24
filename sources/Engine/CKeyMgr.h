#pragma once

enum class KEY
{
	W,
	S,
	A,
	D,
	Z,
	Y,
	R,

	LEFT,
	RIGHT,
	UP,
	DOWN,

	SPACE,
	ENTER,
	ESC,

	LSHIFT,
		
	NUM_1,
	NUM_2,
	NUM_3,
	NUM_4,
	NUM_5,
	NUM_6,
	NUM_7,
	NUM_8,
	NUM_9,
	NUM_0,

	LBTN,
	RBTN,
	WHELLBTN,

	END,
};



enum class KEY_STATE
{
	TAP,		// �� ���� ����
	PRESSED,	// ��� �����ִ� ����
	RELEASED,	// �� �� ����
	NONE,		// �������� ���� ����
};


struct tKeyInfo
{
	KEY_STATE	State;
	bool		PrevPressed;
};


class CKeyMgr
	: public singleton<CKeyMgr>
{
	SINGLE(CKeyMgr);
private:
	vector<tKeyInfo>	m_vecKey;

	Vec2				m_MousePos;		// ���� ���콺 ��ġ
	Vec2				m_MousePrevPos;	// ���� ������ ���콺 ��ġ
	Vec2				m_MouseDir;		// ���콺 ��ǥ�� �̵��� ����

public:
	void Init();
	void Tick();

	KEY_STATE GetKeyState(KEY _Key) { return m_vecKey[(int)_Key].State; }
	Vec2 GetMousePos() { return m_MousePos; }
	Vec2 GetMouseDir() { return m_MouseDir; }
	Vec3 GetRelativeMousePos();

};

