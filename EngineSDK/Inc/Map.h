#pragma once
#include "GameObject.h"

BEGIN(Engine)
class ENGINE_DLL CMap abstract : public CGameObject // 독립적인 객체 생성은 막아도 자식 생성시에는 불릴 수 있도록
{
public:
    enum GAMEOBJ_DATA
    {
      DATA_TERRAIN,
      DATA_WALL,
      DATA_NONANIMAPOBJ,
      DATA_ANIMAPOBJ
    };
public:
    typedef struct MAP_DESC : public CGameObject::GAMEOBJ_DESC
    {
        GAMEOBJ_DATA  DATA_TYPE{};
        const _tchar* FilePath{};
    } MAP_DESC;


protected:
    CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMap(const CMap& Prototype);
    virtual ~CMap() = default;

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


    /*for EDIT to Client*/
    virtual void Set_Model(const _wstring& protoModel)
    {
    }
    GAMEOBJ_DATA Get_Data()
    {
        return  m_DATA_TYPE;
    }
    virtual _wstring Get_ComPonentName()  { return NULL; }

    virtual void Set_buffer(_uint x, _uint y) {};
protected:
    class CGameInstance* m_pGameInstance = {nullptr};
    ID3D11Device* m_pDevice = {nullptr};
    ID3D11DeviceContext* m_pContext = {nullptr};

    /*게임 오브젝트가 생성될때 기본적으로 트랜스 폼을 가지고 있게 한다.*/
    class CTransform* m_pTransformCom = {nullptr};

    GAMEOBJ_DATA m_DATA_TYPE{};

public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void Free() override;
};
END
