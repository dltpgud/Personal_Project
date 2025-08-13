#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Player_StateNode.h"

BEGIN(Client)

class CPlayer_StateMachine : public CBase
{
public:
    struct PLAYER_STATEMACHINE_DESC : public CPlayer_StateNode::PLAYER_INFO_DESC
    {
    };

    enum STATE_NODE
    {
        NODE_IDLE,
        NODE_SWICH,
        NODE_STUN,
        NODE_HIT,
        NODE_RUN,
        NODE_SPRINT,
        NODE_JUMP,
        NODE_RELOAD,
        NODE_SHOOT,
        NODE_FALL,
        NODE_END
    };

protected:
    CPlayer_StateMachine();
    virtual ~CPlayer_StateMachine() = default;

public:
    virtual HRESULT Initialize(void* pArg);

    void StateMachine_Playing(_float fTimeDelta);
private:
   void progress_Move(_float fTimeDelta, _uint* pState) ;

protected:
    CPlayer* m_pParentObject = {};
    class CGameInstance* m_pGameInstance = {};

private:
    vector<CPlayer_StateNode*> m_StateNodes;
    _uint m_iState{};

public:
    static CPlayer_StateMachine* Create(void* pArg);
    virtual void Free();
};

END
