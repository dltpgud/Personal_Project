#pragma once
#include "Transform.h"

BEGIN(Engine)
class CModel;
class CCollider;
class ENGINE_DLL CGameObject abstract : public CBase // 독립적인 객체 생성은 막아도 자식 생성시에는 불릴 수 있도록
{
public:
    typedef struct GAMEOBJ_DESC : public CTransform::TRANSFORM_DESC
    {
        _int  Object_Type{};
        _tchar* ProtoName{};
        _int CuriLevelIndex{};
    }GAMEOBJ_DESC;

    typedef struct PickObj_Desc
    {
        CGameObject* pPickedObj{};
        _float fDis{};
        _vector vPos{};
    } PICKEDOBJ_DESC;


protected:
    CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGameObject(const CGameObject& Prototype);
    virtual ~CGameObject() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();
    virtual HRESULT Render_Shadow() { return S_OK; }

    class CComponent* Find_Component(const _wstring& strComponentTag);
    CTransform* Get_Transform() {return m_pTransformCom;}
    CCollider*  Get_Collider()  {return m_pColliderCom; }

    void Set_Dead(_bool Dead){m_bDead = Dead;}
    _bool Get_Dead(){return  m_bDead;}

    virtual void Set_Model(const _wstring& protoModel, _uint iLevel = 0)
    {
    }
    virtual void Set_InstaceBuffer(const vector<_matrix>& worldmat){}
     _int  Get_ObjectType()
    {
        return m_iObjectType;
    }
 
protected:
    class CGameInstance* m_pGameInstance = {nullptr};
    ID3D11Device* m_pDevice = {nullptr};
    ID3D11DeviceContext* m_pContext = {nullptr};

    _bool m_bDead{};
    _int m_iObjectType{-1};
    _bool m_bClone{};
  
protected:
    map<const _wstring, class CComponent*> m_Components;
    CCollider* m_pColliderCom = {nullptr};
    CTransform* m_pTransformCom = {nullptr};

protected:
    HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag,
                          CComponent** ppOut, void* pArg = nullptr);
public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void Free() override;
};
END
