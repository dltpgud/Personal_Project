#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Player.h"
#include "GameInstance.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CPlayer_StateNode :public CBase
{
public:
    enum DIRECTION : _uint
    {
        DIR_FORWARD = 1 << 0,
        DIR_RIGHT   = 1 << 1,
        DIR_BACK    = 1 << 2,
        DIR_LEFT    = 1 << 3
    };
    enum MOVEMENT : _uint
    {
        MOV_IDLE    = 1 << 4,
        MOV_RUN     = 1 << 5,
        MOV_SPRINT  = 1 << 6,
        MOV_JUMP    = 1 << 7,
        MOV_HIT     = 1 << 8,
        MOV_FALL    = 1 << 9,
        MOV_STURN   = 1 << 10
    };

    enum BEHAVIOR : _uint
    {
        BEH_RELOAD  = 1 << 11,
        BEH_SHOOT   = 1 << 12,
        BEH_SWICH   = 1 << 13,
    };

public:
	struct PLAYER_INFO_DESC
	{
         CPlayer* pParentObject{};
         _uint* pCurWeapon{};
	};

    struct PLAYER_STATE_DESC
    {
        _int iCurMotion[CPlayer::PART_END]{};
        _bool bLoop{};
        _float fPlayTime{1.f};
    };

protected:
	CPlayer_StateNode();
	virtual ~CPlayer_StateNode() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual void State_Enter(_uint* pState);
    virtual _bool State_Processing(_float fTimedelta, _uint* pState);
    virtual _bool State_Exit(_uint* pState);
    virtual void Init_CallBack_Func();
    virtual _bool IsActive(_uint stateFlags) const {return false;};
    virtual void SetActive(_bool active, _uint* pState) {};
    virtual _bool CanEnter(_uint* pState) {return false;}; 
    virtual _bool CheckInputCondition(_uint stateFlags) {return true;};

protected:
    _bool Move_KeyFlage(_uint* pState); 

protected:
	class CGameInstance* m_pGameInstance = { nullptr };
    class CPlayer* m_pParentObject{};
    PLAYER_STATE_DESC m_StateDesc;
    _uint* m_pCurWeapon{};


public:
    static CPlayer_StateNode* Create(void* pArg);
	virtual void Free();
};

END