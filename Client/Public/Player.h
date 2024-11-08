#pragma once
#include "Client_Defines.h"
#include "Actor.h"
BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CPlayerUI;
class CPlayer final : public CActor
{
public:
	enum PARTOBJID { PART_BODY, PART_WEAPON, PART_EFFECT, PART_END };
	enum TYPE {T00, T01, T_END};
	enum STATE {
		STATE_COOLING_LONG, STATE_COOLING_SHORT, STATE_DEATH, STATE_GENERIC_SECONDARY_SHOOT, STATE_GENERIC_STAP, STATE_IDLE, STATE_JUMP_RUN_HIGH
		, STATE_JUMP_RUN_LOW, STATE_JUMP_RUN_MEDIUM, STATE_JUMP_FALL, STATE_JUMP_RUN_LOOP, STATE_JUMP_RUN_START, STATE_SPTINT_FALL, STATE_SPTINT_LOOP,
		STATE_RUN, STATE_SPLINE_RIDE, STATE_SPRINT, STATE_STUN, STATE_SWITCHIN, STATE_WEAPON_THROW02, STATE_WEAPON_THROW0, STATE_HIT_BACK, STATE_HIT_FRONT,
		STATE_HIT_HEAVY, STATE_HIT_LEFT, STATE_HIT_RIGHT,

		STATE_COOLING_LONG2, STATE_COOLING_SHORT2, STATE_DEATH2, STATE_GENERIC_SECONDARY_SHOOT2, STATE_GENERIC_STAP2, STATE_IDLE2, STATE_JUMP_RUN_HIGH2
		, STATE_JUMP_RUN_LOW2, STATE_JUMP_RUN_MEDIUM2, STATE_JUMP_FALL2, STATE_JUMP_RUN_LOOP2, STATE_JUMP_RUN_START2, STATE_SPTINT_FALL2, STATE_SPTINT_LOOP2,
		STATE_RUN2, STATE_SPLINE_RIDE2, STATE_SPRINT2, STATE_STUN2, STATE_SWITCHIN2, STATE_WEAPON_THROW022, STATE_WEAPON_THROW012, STATE_HIT_BACK2, STATE_HIT_FRONT2,
		STATE_HIT_HEAVY2, STATE_HIT_LEFT2, STATE_HIT_RIGHT2,

		STATE_HENDGUN_RELOAD,
		STATE_HENDGUN_SHOOT,
		STATE_ASSAULTRIFlLE_RELOAD,
		STATE_ASSAULTRIFlLE_SHOOT,
		STATE_MissileGatling_HENDPOSE,
		STATE_MissileGatling_RELOAD,
		STATE_MissileGatling_SHOOT,
		STATE_HEAVYCROSSBOW_HENDPOSE,
		STATE_HEAVYCROSSBOW_RELOAD,
		STATE_HEAVYCROSSBOW_SHOOT

	};
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	/* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
	virtual HRESULT Initialize_Prototype() override;

	/* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
	/* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void HIt_Routine()override;
	virtual void Stun_Routine()override;
	
	virtual _float Weapon_Damage() override;
	const _float4x4* Get_CameraBone();
	void Key_Input(_float fTimeDelta);
	
	void Choose_Weapon(const _uint& WeaponNum);
	_uint Get_Bullet();
	_uint Get_MaxBullte();
	_bool Get_jump() { return m_bJump; }
	void Set_HitLock(_bool Lock);

private:
	_uint					m_Type = {};
	_uint					m_iWeaponType = {};
	_bool					m_bturn = { false };
	_bool					m_bchange = { false };
	_bool					m_bJump = { false };
	_bool					m_bDoubleJump = { false };
	_bool					m_bHitLock = { false };
	_bool					m_bFallLock = { false };
	_float					m_DemageCellTime = { 0.f};
	CPlayerUI*				m_pPlayerUI = { nullptr };
	const _float4x4*		m_pCameraBone = { nullptr };
	_uint m_iJumpCount = 0;

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
END
