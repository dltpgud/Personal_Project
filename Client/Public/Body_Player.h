#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)

END

BEGIN(Client)
enum BODY_TYPE {T00, T01, T_END};
class CBody_Player: public CPartObject
{
public: 
	enum BODY_STATE {
            BODY_IDLE,
            BODY_SWICH,
            BODY_STUN,
            BODY_RUN,
            BODY_SPRINT,
            BODY_JUMP_RUN_LOOP,

            BODY_JUMP_FALL,
            BODY_COOLING_LONG,
            BODY_COOLING_SHORT,
            BODY_DEATH,
            BODY_GENERIC_SECONDARY_SHOOT,
            BODY_GENERIC_STAP,
            BODY_SPTINT_FALL,
            BODY_SPTINT_LOOP,
            BODY_WEAPON_THROW1,
            BODY_WEAPON_THROW2,

            BODY_HENDGUN_RELOAD = 52,
            BODY_HENDGUN_SHOOT,
            BODY_ASSAULTRIFlLE_RELOAD,
            BODY_ASSAULTRIFlLE_SHOOT,
            BODY_MissileGatling_HENDPOSE,
            BODY_MissileGatling_RELOAD,
            BODY_MissileGatling_SHOOT,
            BODY_HEAVYCROSSBOW_HENDPOSE,
            BODY_HEAVYCROSSBOW_RELOAD,
            BODY_HEAVYCROSSBOW_SHOOT,
		    BODY_END
	};

private:
 enum BODY_MOTION{// HIT ¸Á°¡Áü, 15,11,8,7,32,33,34,37,41,47~ ³¡°¡Áö ¾Ö´Ô ¸Á°¡Áü
        STATE_COOLING_LONG, STATE_COOLING_SHORT, STATE_DEATH, STATE_GENERIC_SECONDARY_SHOOT, STATE_GENERIC_STAP,
        STATE_IDLE1, STATE_JUMP_RUN_HIGH, STATE_JUMP_RUN_LOW, STATE_JUMP_RUN_MEDIUM, STATE_JUMP_FALL,
        STATE_JUMP_RUN_LOOP, STATE_JUMP_RUN_START, STATE_SPTINT_FALL, STATE_SPTINT_LOOP, STATE_RUN, STATE_SPLINE_RIDE,
        STATE_SPRINT, STATE_STUN, STATE_SWITCHIN, STATE_WEAPON_THROW1, STATE_WEAPON_THROW3, STATE_HIT_BACK,
        STATE_HIT_FRONT, STATE_HIT_HEAVY, STATE_HIT_LEFT, STATE_HIT_RIGHT, STATE_COOLING_LONG2, STATE_COOLING_SHORT2,
        STATE_DEATH2, STATE_GENERIC_SECONDARY_SHOOT2, STATE_GENERIC_STAP2, STATE_IDLE2, STATE_JUMP_RUN_HIGH2,
        STATE_JUMP_RUN_LOW2, STATE_JUMP_RUN_MEDIUM2, STATE_JUMP_FALL2, STATE_JUMP_RUN_LOOP2, STATE_JUMP_RUN_START2,
        STATE_SPTINT_FALL2, STATE_SPTINT_LOOP2, STATE_RUN2, STATE_SPLINE_RIDE2, STATE_SPRINT2, STATE_STUN2,
        STATE_SWITCHIN2, STATE_WEAPON_THROW2, STATE_WEAPON_THROW4, STATE_HIT_BACK2, STATE_HIT_FRONT2,
        STATE_HIT_HEAVY2, STATE_HIT_LEFT2, STATE_HIT_RIGHT2};
 
public : 
	typedef struct BODY_PLAYER_DESC : CPartObject::PARTOBJECT_DESC
	{

	}BODY_PLAYER_DESC;

private:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& Prototype);
	virtual ~CBody_Player() = default;

public:
	
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

    void Set_Animation(_int Body, _int Index, _bool IsLoop);
    _bool Play_Animation(_float fTimeDelta);
    void BodyCallBack(_int Body, _uint AnimIdx, _int Duration, function<void()> func);

private:
    _int BodyMotion(_int Body, _int Motion);

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END