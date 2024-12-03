#pragma once
#include "Client_Defines.h"
#include "Actor.h"
BEGIN(Engine)

END

BEGIN(Client)
class CBillyBoom final : public CActor
{
public:
    enum PARTOBJID
    {
        PART_BODY,
        PART_INTRO,
        PART_EFFECT,
        PART_END
    };
    enum STATE
    {
      ST_AIM_Left,
      ST_Idle,
      ST_AIM_Left45,
      ST_AIM_Middle,
      ST_AIM_Right,
      ST_AIM_Right45,
      ST_AIM_Antenne_In,
      ST_AIM_Antenne_Out,
      ST_Barre_In,
      ST_Barre_PreShoot,
      ST_Barre_Shoot,
      ST_Bash_PreShoot,
      ST_Bash_Shoot,
      ST_HIT_Front,
      ST_Intro,
      ST_Laser_In,
      ST_Laser_PreShoot,
      ST_Laser_ShootLoop,
      ST_Lever_Activate,
      ST_Lever_In,
      ST_Lever_Out,
      ST_Run_Front,
      ST_ShockWave_In,
      ST_ShockWave_Out,
      ST_ShockWave_PreShoot,
      ST_ShockWave_Shoot,
      ST_Stun_Pose,
      ST_Stun_Recover,
      ST_Stun_Start,
      ST_Comp_Idle,
      ST_Comp_Poke_Back,
      ST_Comp_Poke_Front,
      ST_Comp_Poke_Left,
      ST_Comp_Poke_Right
    };                   

private:
    CBillyBoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBillyBoom(const CBillyBoom& Prototype);
    virtual ~CBillyBoom() = default;

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
    virtual void Dead_Routine(_float fTimeDelta) override;
    virtual void Stun_Routine()override;

    void NON_intersect(_float fTimedelta);
    void Intro_Routine(_float fTimedelta);
    void Change_State(_float fTimedelta);

    void Set_Barre_Shoot(_float fTimedelta);
    void Set_Laser(_float fTimedelta);
    void Set_ShockWave(_float fTimedelta);
    void Set_Bash(_float fTimedelta);
    void Set_State_From_Body();
private:
    HRESULT Add_Components();
    HRESULT Add_PartObjects();
private:
    _bool            m_bIntro = { true };
    class CBossHPUI* m_pHP = { nullptr };
    _int	         m_iPrAttack[4] = { -1, -1, -1, -1 };
    _int             m_iSkillTime = 0;
    _bool               m_bSkill = false;
    _bool            m_bHit = false;
    _float          m_fHitTimeSum = { 0.f };
    _bool m_bStart = true;
    _float m_DeadTimeSum = { 0.f };
    _bool m_bBoom{ false };
    _bool m_bDeadSound{ false };
    _bool m_bPade{};
public:
    static CBillyBoom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END
