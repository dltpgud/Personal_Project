#pragma once
#include "GameObject.h"
#include "Collider.h"
BEGIN(Engine)
class CNavigation;
class CCollider;
class CShader;
class ENGINE_DLL CSkill abstract : public CGameObject
{
public:
 
    enum ATCORTYPE {PLAYER, NOMAL_MONSTER, BOSS_MONSTER};
    enum SKill { STYPE_NOMAL, STYPE_STURN, STYPE_BERRLE, STYPE_SHOCKWAVE, STYPE_MISSILE, STYPE_LASER, STYPE_END};
    enum COLOR {CSTART, CEND, COLOR_END};
public:
    typedef struct Skill_DESC : public CGameObject::GAMEOBJ_DESC
    {
        _vector vPos{};
        _int    iDamage{};
        _float  fLifeTime{};
        _uint   iActorType{};
        _uint   iSkillType{};
        _float4 fClolor[CSkill::COLOR::COLOR_END];
    }Skill_DESC;

protected:
     CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
     CSkill(const CSkill& Prototype);
     virtual ~CSkill() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();
    virtual void Dead_Rutine() {}
    virtual _bool Comput_SafeZone(_fvector vPlayerPos) {return false;}
    _int Get_Damage();
    _uint Get_SkillType();
    _uint Get_ActorType();

protected:
    _uint        m_iActorType{ 0 };
    _int         m_iDamage = {};
    CShader*     m_pShaderCom = { nullptr };
    CNavigation* m_pNavigationCom = { nullptr };
    _float       m_fLifeTime{0.f};
    _vector      m_vPos{};
    _float       m_fTimeSum{0.f};
    _uint        m_iSkillType = { STYPE_NOMAL };
    _float4      m_Clolor[COLOR_END]{};

public:
        virtual CGameObject* Clone(void* pArg) = 0;
        virtual void Free() override;
};
END


