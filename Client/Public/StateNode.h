#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CModel;
END

BEGIN(Client)

class CStateNode :public CBase
{
public:
    struct STATENODE_DESC
    {
        class CModel* pParentModel = {nullptr}; // 모델
        _int iCurrentState = 0; // 현재 상태 인덱스
        _int iNextStateIdx = -1; // 다음 상태 인덱스
        _bool* bCondition = nullptr; 
        _bool bIsLoop{}; 
        _float fPlayAniTime{1.f};             
    };

protected:
    CStateNode();
    virtual ~CStateNode() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual void State_Enter();
    virtual _bool State_Processing(_float fTimeDelta);
    virtual _bool State_Exit(_int* NextIndex);

private:
    class CGameInstance* m_pGameInstance = {nullptr};

    STATENODE_DESC m_StateDesc;

    _bool m_bFinished = false;

public:
    static CStateNode* Create(void* pDesc);
    virtual void Free();
};

END