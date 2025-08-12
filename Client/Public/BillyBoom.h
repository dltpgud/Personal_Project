#pragma once
#include "Client_Defines.h"
#include "Monster.h"
BEGIN(Engine)

END

BEGIN(Client)
class CBillyBoom final : public CMonster
{
public:
    enum STATE 
    {
        ST_BARRE,
        ST_BASH,
        ST_LASER,
        ST_SHOCKWAVE,
        ST_IDLE,
        ST_HIT,
        ST_DEAD,
        ST_MOVE,
        ST_INTRO,
        ST_END
    };         

private:
    CBillyBoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBillyBoom(const CBillyBoom& Prototype);
    virtual ~CBillyBoom() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual void HIt_Routine()override;
    virtual void Dead_Routine() override;
    virtual void Set_CurrentHP(_float CurrentHp) override;
   
    void Change_State(_int state);

    void Set_bSkill(_bool skill)
    {
        m_bSkill = skill;
    }

    _int Get_NextSkil()
    {
        return m_iNextSkill;
    }

    void Set_bFinishIntro(_bool bFinishIntro)
    {
        m_bFinishIntro = bFinishIntro;
    }

private:
    HRESULT Add_Components();
    HRESULT Add_PartObjects();
    void    Change_Pattern();

private:
    class CBossHPUI* m_pHP = { nullptr };
    _int	         m_iPrAttack[4] = { -1, -1, -1, -1 };
    _int             m_iSkillTime = 0;
    _int             m_iNextSkill {-1};
    _bool            m_bSkill = false;
    _bool            m_bFinishIntro = false;

    mt19937 m_Rng{random_device{}()};
    uniform_int_distribution<int> m_Dist{0, 3};

public:
    static CBillyBoom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END
