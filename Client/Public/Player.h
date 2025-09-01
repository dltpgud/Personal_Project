#pragma once
#include "Client_Defines.h"
#include "Actor.h"
#include "Weapon.h"
BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CPlayer_HpUI;
class CWeapon;
class CPlayer final : public CActor
{
public:

	enum PARTOBJID { PART_BODY, PART_WEAPON, PART_END };

    enum PLAYER_FLAGS : _uint
    {
          FLAG_CHANGE  = 1 << 0, 
          FLAG_HIT     = 1 << 1,   
          FLAG_STURN   = 1 << 2,
          FLAG_UPDATE  = 1 << 3,  
          FLAG_HEALTH  = 1 << 4, 
          FLAG_KEYLOCK = 1 << 5,
    };
       
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void HIt_Routine()override;
	virtual void Stun_Routine()override;

public:
	const _float4x4* Get_CameraBone();
    void WeaponCallBack(_int WeaPonType, _uint AnimIdx, _int Duration, function<void()> func);
    void BodyCallBack(_int Body, _uint AnimIdx, _int Duration, function<void()> func);
    void Set_PartObj_Set_Anim(_int Part, _int Index, _bool IsLoop);
    _bool Set_PartObj_Play_Anim(_int Part, _float fTimeDelta, _float fPlayAniTime = 1.f);

	void Choose_Weapon(const _uint& WeaponNum);
    CWeapon::WEAPON_NODE_DESC Get_Weapon_Info() const;

    
    void SetFlag(PLAYER_FLAGS flag, _bool value)
    {
        if (value)
        {
            m_iState = flag;
            m_iState |= FLAG_UPDATE;
        }
        else
            m_iState &= ~flag; 
    }

    void AddFlag(PLAYER_FLAGS flag)
    {
        m_iState |= flag;
    }

    _bool GetFlag(PLAYER_FLAGS flag) const
    {
        return (m_iState & flag) != 0;
    }

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();

private:
    class CPlayer_StateMachine* m_pStateMachine = {nullptr};
    CPlayer_HpUI*               m_pPlayerUI = {nullptr};
    const _float4x4*            m_pCameraBone = {nullptr};

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
END
//////////////////////////////////MODEL_ANIM_DATA//////////////////////////////////////////////
/* enum STATE {
    STATE_COOLING_LONG,
    STATE_COOLING_SHORT,
    STATE_DEATH,
    STATE_GENERIC_SECONDARY_SHOOT,
    STATE_GENERIC_STAP,
    STATE_IDLE,
    STATE_JUMP_RUN_HIGH,
    STATE_JUMP_RUN_LOW,
    STATE_JUMP_RUN_MEDIUM,
    STATE_JUMP_FALL,
    STATE_JUMP_RUN_LOOP,
    STATE_JUMP_RUN_START,
    STATE_SPTINT_FALL,
    STATE_SPTINT_LOOP,
    STATE_RUN,
    STATE_SPLINE_RIDE,
    STATE_SPRINT,
    STATE_STUN,
    STATE_SWITCHIN,
    STATE_WEAPON_THROW02,
    STATE_WEAPON_THROW0,
    STATE_HIT_BACK,
    STATE_HIT_FRONT,
    STATE_HIT_HEAVY,
    STATE_HIT_LEFT,
    STATE_HIT_RIGHT,
    STATE_COOLING_LONG2,
    STATE_COOLING_SHORT2,
    STATE_DEATH2,
    STATE_GENERIC_SECONDARY_SHOOT2,
    STATE_GENERIC_STAP2,
    STATE_IDLE2,
    STATE_JUMP_RUN_HIGH2,
    STATE_JUMP_RUN_LOW2,
    STATE_JUMP_RUN_MEDIUM2,
    STATE_JUMP_FALL2,
    STATE_JUMP_RUN_LOOP2,
    STATE_JUMP_RUN_START2,
    STATE_SPTINT_FALL2,
    STATE_SPTINT_LOOP2,
    STATE_RUN2,
    STATE_SPLINE_RIDE2,
    STATE_SPRINT2,
    STATE_STUN2,
    STATE_SWITCHIN2,
    STATE_WEAPON_THROW022,
    STATE_WEAPON_THROW012,
    STATE_HIT_BACK2,
    STATE_HIT_FRONT2,
    STATE_HIT_HEAVY2,
    STATE_HIT_LEFT2,
    STATE_HIT_RIGHT2,
    STATE_HENDGUN_RELOAD,
    STATE_HENDGUN_SHOOT,
    STATE_ASSAULTRIFlLE_RELOAD,
    STATE_ASSAULTRIFlLE_SHOOT,
    STATE_MissileGatling_HENDPOSE,
    STATE_MissileGatling_RELOAD,
    STATE_MissileGatling_SHOOT,
    STATE_HEAVYCROSSBOW_HENDPOSE,
    STATE_HEAVYCROSSBOW_RELOAD,
    STATE_HEAVYCROSSBOW_SHOOT,
    STATE_END
};*/