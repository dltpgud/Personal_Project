#pragma once
#include "ContainerObject.h"
#include "Collider.h"
BEGIN(Engine)
class CNavigation;
class CCollider;
class ENGINE_DLL CActor abstract : public CContainerObject
{
    public:
        typedef struct Actor_DESC : public CContainerObject::CONTAINEROBJECT_DESC
        {
            _float fHP{};
            _float fMAXHP{};
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
        void Set_bColl(_bool Coll) { m_bColl = Coll; }
        virtual void HIt_Routine();


        void Set_State(_uint st)
        {
            m_iState = st;
        }
        _uint Get_State() { return m_iState; }
    protected:

        CNavigation* m_pNavigationCom = { nullptr };
        _float m_fHP;
        _float m_fMAXHP;
        _bool m_bColl = { false };
        _uint m_iState = {};
    public:
        virtual CGameObject* Clone(void* pArg) = 0;
        virtual void Free() override;
    };
    END


