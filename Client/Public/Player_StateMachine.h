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

    enum class Result
    {
        Running,
        Finished,
        None
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
        NODE_END
    };

protected:
    CPlayer_StateMachine();
    virtual ~CPlayer_StateMachine() = default;

public:
    virtual HRESULT Initialize(void* pArg);

    Result StateMachine_Playing(_float fTimeDelta);
    void   Set_ADD_Flage(_uint Flags);

private:
   void progress_Move(_float fTimeDelta, _uint stateFlags) const;
  

protected:
    CPlayer* m_pParentObject = {};
    class CGameInstance* m_pGameInstance = {nullptr};
    vector<CPlayer_StateNode*> m_StateNodes;

    
    _bool m_ChangeNode{};
    _uint m_iState{};

public:
    static CPlayer_StateMachine* Create(void* pArg);
    virtual void Free();
};

END
