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
        TYPE_BOOM_BOT, TYPE_GUN_PAWN, TYPE_JET_FLY, TYPE_MECANOBOT,TYPE_BOSS, TYPE_END
        
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
        virtual void Priority_Update(_float fTimeDelta);
        virtual void Update(_float fTimeDelta);
        virtual void Late_Update(_float fTimeDelta);
        virtual HRESULT Render();

public:
        //상태 관련
        void Check_Coll();
        void Set_bStun(_bool Coll) { m_bStun = Coll; }
        _bool Get_bStun() {return  m_bStun; }
        void Set_State(_uint st)
        {
            m_iState = st;
        }
        _uint Get_State() { return m_iState; }
        virtual _bool Get_bJump() { return false; }
        virtual void HIt_Routine() {};
        virtual void Dead_Routine() {};
        virtual void Stun_Routine() {};
      

        //네비 관련
        void Set_NavigationType(_uint i);
        _int Get_CurrentCell_Type();
        void Find_CurrentCell();
        _float Get_fY() { return m_fY; }
        void Height_On_Cell(_float3* fPos);
        void Set_onCell(_bool bonCell) { m_bOnCell = bonCell; }
        _bool Get_onCell(){ return m_bOnCell;}
        void Clear_CNavigation(_tchar* tFPath); // 플레이어 씬전환 후 새로운쎌 부여
        void Set_Taget(_vector Taget);
        CNavigation* Get_Navi()
        {
            return m_pNavigationCom;
        }
        void Is_onDemageCell(_float fTimeDelta);
        //체력관련
        virtual void Set_CurrentHP(_float CurrentHp)
        {
            m_fHP -= CurrentHp;
        }
        void Set_HealthCurrentHP(_float Health);
        _bool IsFullHP() const { return m_fHP == m_fMAXHP; }
        virtual _float Weapon_Damage() { return 0.f; }
       

    protected:

         CNavigation* m_pNavigationCom = { nullptr };
        _float  m_fHP;
        _float  m_fMAXHP;
        _uint   m_iState = {};
        _float	m_fY{ 0.f };
        _float  m_FixY{ 0.f };
        _bool   m_bOnCell = { false };
        _uint   m_iRim{}; // 림 연산 할꺼 말꺼
        _bool   m_bStun = {false};

    public:
        virtual CGameObject* Clone(void* pArg) = 0;
        virtual void Free() override;
    };
    END


