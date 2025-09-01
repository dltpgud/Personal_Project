#pragma once
#include "ContainerObject.h"
#include "Collider.h"
BEGIN(Engine)
class CNavigation;
class CCollider;

class ENGINE_DLL CActor abstract : public CContainerObject
{
public:
    enum ACTOR_TYPE
    {
        TYPE_PLAYER, TYPE_MONSTER,TYPE_NPC
    };
    enum Flag
    {
        Trigger_Terrain = 2 << 1  
       
    };

public:
    typedef struct Actor_DESC : public CContainerObject::CONTAINEROBJECT_DESC
    {
       _uint  iType{}; 
       _int   iHP{}; 
       _float fFixY{};
       _bool  bOnCell{};
       _uint  iState{};
    }Actor_DESC;

protected:
     CActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
     CActor(const CActor& Prototype);
     virtual ~CActor() = default;

public:
     virtual HRESULT Initialize_Prototype();
     virtual HRESULT Initialize(void* pArg);
     virtual void Priority_Update(_float fTimeDelta);
     virtual void Update(_float fTimeDelta);
     virtual void Late_Update(_float fTimeDelta);
     virtual HRESULT Render();
     virtual void HIt_Routine() {};
     virtual void Dead_Routine() {};
     virtual void Stun_Routine() {};

 public:
     void   Check_Coll();
     _uint  Get_Type() {return m_iType;}


     void   Find_CurrentCell();
     _float Get_fY() { return m_fY; }
     void   Height_On_Cell(_float3* fPos);
     void   Set_onCell(_bool bonCell) { m_bOnCell = bonCell; }
     _bool  Get_onCell(){ return m_bOnCell;}
     void   Clear_CNavigation(_tchar* tFPath);
  
     CNavigation* Get_Navigation() { return m_pNavigationCom; }
     virtual void Set_CurrentHP(_int CurrentHp){m_iHP -= CurrentHp;}
     void Set_HealthCurrentHP(_int Health);
     _bool IsFullHP() const { return m_iHP == m_iMAXHP;}

     void SetTriggerFlag(Flag flag, _bool value)
     {
            if (value)
            {
                m_iFlag |= flag;
            }
            else
                m_iFlag &= ~flag;
     }
     _bool GetTriggerFlag(Flag flag) const
     {
            return (m_iFlag & flag) != 0;
     }

protected:
     CNavigation* m_pNavigationCom = { nullptr };
     _int         m_iHP{};
     _int         m_iMAXHP{};
     _uint        m_iState = {};
     _float       m_fY{0.f};
     _float       m_FixY{0.f};
     _bool        m_bOnCell = {false};
     _uint        m_iType = {};
     _uint        m_iFlag = {};
     _uint        m_iRim{}; 
    
public:
        virtual CGameObject* Clone(void* pArg) = 0;
        virtual void Free() override;
};
    END


