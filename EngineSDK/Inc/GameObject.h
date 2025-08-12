#pragma once
#include "Transform.h"

BEGIN(Engine)
class CModel;
class CCollider;
class ENGINE_DLL CGameObject abstract : public CBase // 독립적인 객체 생성은 막아도 자식 생성시에는 불릴 수 있도록
{
public:
    enum GAMEOBJ_TYPE
    {
        ACTOR,
        SKILL,
        MAP,
        ITEM,
        NPC,
        UI,
        CAMERA,
        GAMEOBJ_TYPE_END
    };
 
public:
    typedef struct GAMEOBJ_DESC : public CTransform::TRANSFORM_DESC
    {
        _int  Object_Type{};
        _tchar* ProtoName{};
        _int CuriLevelIndex{};
        _float Buffer[2];
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
    virtual HRESULT Render_Height()
    {
        return S_OK;
    }
    
    virtual CModel* Get_Model() {
        return nullptr;
    };

    class CComponent* Find_Component(const _wstring& strComponentTag);
    CTransform* Get_Transform()
    {
        return m_pTransformCom;
    }


    void Set_Dead(_bool Dead)
    {
        m_bDead = Dead;
    }

    _bool Get_Dead()
    {
        return  m_bDead;
    }

    virtual _float Check_Pick(_vector RayPos, _vector RayDir)
    {
        return _float{ 0xffff };
    }

    /*for EDIT to Client*/
    virtual void Set_Model(const _wstring& protoModel, _uint iLevel = 0)
    {
    }
    virtual void Set_InstaceBuffer(const vector<_matrix>& worldmat)
    {
    }
     _int  Get_ObjectType()
    {
        return m_iObjectType;
    }
    virtual _wstring Get_ComPonentName()
    {
        return L"";
    }
    virtual _tchar* Get_ProtoName() { return nullptr; }
    virtual void Set_Buffer(_uint x, _uint y) {};
    virtual _uint Get_Scalra() { return 0; };
    virtual _float Get_Scalra_float() { return 0.f; };
    virtual void Set_Scalra_uint(_uint scalra) {};
    virtual void Set_Scalra_float(_float scalra) {};

  virtual _float check_BoxDist(_vector RayPos, _vector RayDir) { return _float(0xffff); }
  virtual CCollider* Get_Collider() { return m_pColliderCom; };
 
  protected:
    class CGameInstance* m_pGameInstance = {nullptr};
    ID3D11Device* m_pDevice = {nullptr};
    ID3D11DeviceContext* m_pContext = {nullptr};
    class CTransform* m_pTransformCom = {nullptr};
    _bool m_bDead{};
    _int m_iObjectType{-1};
    _bool m_bClone{};
    CCollider* m_pColliderCom = { nullptr };
 
protected:
    map<const _wstring, class CComponent*> m_Components;

protected:
    HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag,
                          CComponent** ppOut, void* pArg = nullptr);
public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void Free() override;
};
END
