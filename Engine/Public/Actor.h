#pragma once
#include "ContainerObject.h"
#include "Collider.h"
BEGIN(Engine)
class CNavigation;
class CCollider;
class ENGINE_DLL CActor abstract : public CContainerObject
{
public:
    enum ACTOR_MONSTER_TYPE {
        TYPE_BOOM_BOT, TYPE_GUN_PAWN, TYPE_JET_FLY, TYPE_END
        
    };

    enum ACTOR_NPC_TYPE { TYPE_HEALTH_BOT };

    public:
        typedef struct Actor_DESC : public CContainerObject::CONTAINEROBJECT_DESC
        {
         
        }Actor_DESC;

    protected:
        CActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
        CActor(const CActor& Prototype);
        virtual ~CActor() = default;

    public:
        virtual HRESULT Initialize_Prototype();
        virtual HRESULT Initialize(void* pArg);
        virtual _int Priority_Update(_float fTimeDelta);
        virtual void Update(_float fTimeDelta);
        virtual void Late_Update(_float fTimeDelta);
        virtual HRESULT Render();

public:
        //상태 관련
        void Set_bColl(_bool Coll) { m_bColl = Coll; }
        void Set_State(_uint st)
        {
            m_iState = st;
        }
        _uint Get_State() { return m_iState; }
        virtual void HIt_Routine(_float fTimeDelta) {};
        virtual void Dead_Routine(_float fTimeDelta) {};
        virtual void Stun_Routine() {};
      
        virtual HRESULT Make_Bullet(_vector vPos) { return S_OK; };

        //Cell 관련
        void Set_NavigationType(_uint i);
        void Find_CurrentCell();
        _float Get_fY() { return m_fY; }
        void Height_On_Cell(_float3* fPos);

        //체력관련
        void Set_CurrentHP(_float CurrentHp) { m_fHP -= CurrentHp; }
        void Set_HealthCurrentHP(_float Health) { m_fHP += Health; }
        _bool IsFullHP() const { return m_fHP == m_fMAXHP; }
        virtual _float Weapon_Damage() { return 0.f; }
     
    protected:

         CNavigation* m_pNavigationCom = { nullptr };
        _float  m_fHP;
        _float  m_fMAXHP;
        _bool   m_bColl = { false };
        _uint   m_iState = {};
        _float	m_fY{ 0.f };
        _float  m_FixY{ 0.f };
        _bool   m_bOnCell = { false };
        _uint   m_iRim{}; // 림 연산 할꺼 말꺼

    public:
        virtual CGameObject* Clone(void* pArg) = 0;
        virtual void Free() override;
    };
    END


