#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "StateNode.h"
#include "Monster.h"
#include "PartObject.h"
BEGIN(Engine)
class CPartObject;
END

BEGIN(Client)

class CStateMachine : public CBase
{
public:
    struct STATEMACHINE_DESC
    {
        _uint iBaseIndex = 0;
        class CModel* pParentModel = {};
        class CMonster* pParentObject = {};
        class CPartObject* pParentPartObject = {};
        _int iNextMachineIdx = -1;
        _uint* iRim;
    };

    enum class Result
    {
        Running,  
        Finished, 
        None     
    };

    enum Flags : unsigned char
    {
        ACTIVE   = 1 << 0,  
        PAUSED   = 1 << 1, 
        FINISHED = 1 << 2 
    };

protected:
    CStateMachine();
    virtual ~CStateMachine() = default;

protected:
    virtual HRESULT Initialize(void* pArg);
    virtual void Init_CallBack_Func() {};

public:
    virtual Result StateMachine_Playing(_float fTimeDelta, RIM_LIGHT_DESC* pRim);
    virtual void Reset_StateMachine(RIM_LIGHT_DESC* pRim);
   

public:
    _int Get_NextMachineIndex()
    {
        return m_iNextMachineIdx;
    }
    void Set_NextMachineIndex(_int NextIdx)
    {
        m_iNextMachineIdx = NextIdx;
    }

    void Set_NextNodeIndex(_int NextIdx)
    {
        m_iNextIndex = NextIdx;
    }

    void SetFlag(Flags flag)
    {
           m_Flags |= flag;
    }
    void ClearFlag(Flags flag)
    {
        m_Flags &= ~flag;
    }
    bool HasFlag(Flags flag) const
    {
        return (m_Flags & flag) != 0;
    }

protected:
    class CModel* m_pParentModel = {nullptr};
    class CGameInstance* m_pGameInstance = {nullptr};
    class CMonster* m_pParentObject = {};
    class CPartObject* m_pParentPartObject = {};
    vector<CStateNode*> m_StateNodes;

    _int m_iNextMachineIdx = -1;
    _uint m_iBaseIndex = 0;

    _int m_iCurIndex = -1;
    _int m_iNextIndex = 0;

    _ubyte m_Flags = 0;
    FMOD::Channel* m_pChannel = nullptr;

public:
    static CStateMachine* Create(void* pArg);
    virtual void Free();
};

END
