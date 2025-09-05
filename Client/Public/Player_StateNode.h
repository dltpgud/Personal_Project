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
    virtual void State_Enter(_uint* pState, _uint* pPreState);// 상태에 진입
    virtual _bool State_Processing(_float fTimedelta, _uint* pState, _uint* pPreState); //상태 진행
    virtual _bool State_Exit(_uint* pState); // 상태 탈출
    virtual void Init_CallBack_Func(); // 초기화할 콜백
    virtual _bool IsActive(_uint stateFlags) const {return false;}; // 플래그가 활성화 되어있는지 확인
    virtual void SetActive(_bool active, _uint* pState) {}; // 플래그 설정
    virtual _bool CanEnter(_uint* pState) {return false;};  // 상태 진입 조건확인
    virtual _bool CheckInputCondition(_uint stateFlags) {return true;}; // 상태 진행중 탈출 조건

protected:
    _bool Move_KeyFlage(_uint* pState);  // 방향 플래그 설정



#ifdef _DEBUG
    void CheckState(_uint* State) const;
    _bool IsFlagOn(_uint* State,  _uint flag) const
    {
        return (*State & flag) != 0;
    }
#endif

protected:
	class CGameInstance* m_pGameInstance = { nullptr };
    class CPlayer* m_pParentObject{};
    PLAYER_STATE_DESC m_StateDesc;
    _uint* m_pCurWeapon{};
    FMOD::Channel* m_pChannel = nullptr;

public:
    static CPlayer_StateNode* Create(void* pArg);
	virtual void Free();
};

END