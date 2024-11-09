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
    enum GAMEOBJ_DATA
    {
        DATA_TERRAIN,
        DATA_WALL,
        DATA_NONANIMAPOBJ,
        DATA_DOOR,
        DATA_CHEST,
        DATA_MONSTER,
        DATA_NPC,
        DATA_CAMERA
    };
public:
    typedef struct GAMEOBJ_DESC : public CTransform::TRANSFORM_DESC
    {
        _int  DATA_TYPE{};
        const _tchar* FilePath{};
        _tchar* ProtoName{};
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
    /* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
    virtual HRESULT Initialize_Prototype();

    /* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
    /* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한다. */
    virtual HRESULT Initialize(void* pArg);
    virtual _int Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();
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
     _int  Get_Data()
    {
        return m_DATA_TYPE;
    }
    virtual _wstring Get_ComPonentName() { return NULL; }
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

    /*게임 오브젝트가 생성될때 기본적으로 트랜스 폼을 가지고 있게 한다.*/
    class CTransform* m_pTransformCom = {nullptr};

    _bool m_bDead{};

    _int m_DATA_TYPE{-1};
   
    _bool m_bClone{};

    CCollider* m_pColliderCom = { nullptr };

protected:
    /*컴포넌트 사본을 저장하는 맵컨테이너*/
    map<const _wstring, class CComponent*> m_Components;

protected:
    HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag,
                          CComponent** ppOut, void* pArg = nullptr);


public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void Free() override;
};
END
