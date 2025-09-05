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
        _uint* iState;
    };

    enum STATE_NODE
    {
        NODE_HEALTH,
        NODE_HIT, 
        NODE_STUN,
        NODE_FALL,

        NODE_SHOOT,
        NODE_RELOAD,
        NODE_SWICH,

        NODE_JUMP,
        NODE_SPRINT,
        NODE_RUN,
        NODE_IDLE,
     
        NODE_END
    };


protected:
    CPlayer_StateMachine();
    virtual ~CPlayer_StateMachine() = default;

public:
    void    ResetMachine(); // 상태 머신 초기화
    void    StateMachine_Playing(_float fTimeDelta); // 상태진행

private:
   HRESULT  Initialize(void* pArg);
   void     progress_Move(_float fTimeDelta, _uint* pState) ;
   void     Check_UIState(_uint* pPreState) const;
   HRESULT  Initialize_StateUI(_uint* CurWeapon);

protected:
    CPlayer* m_pParentObject = {};
    class CGameInstance* m_pGameInstance = {};

private:
    vector<CPlayer_StateNode*> m_StateNodes;
    _uint* m_iState{};
    _uint m_iPreviousState{}; 
    _uint m_iLastActiveState{}; 

public:
    static CPlayer_StateMachine* Create(void* pArg);
    virtual void Free();
};

END
