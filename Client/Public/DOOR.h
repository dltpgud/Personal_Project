#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CInteractiveUI;
class CPlayer;
class CDOOR final : public CGameObject
{
	enum State {
	OPEN, ClOSE
	};
	enum State2 { OPEN2, IDLE,ClOSE2};
private:
	CDOOR(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDOOR(const CDOOR& Prototype);
	virtual ~CDOOR() = default;

public:
	/* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
	virtual HRESULT Initialize_Prototype() override;

	/* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
	/* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;	
	    virtual HRESULT Render_Shadow() override;
	virtual void Set_Model(const _wstring& protoModel, _uint ILevel) override;
	virtual CModel* Get_Model() override { return m_pModelCom; }
	virtual void Set_Buffer(_uint x, _uint y)override;

	HRESULT Add_StageDoorLight();
	HRESULT Add_BossDoorLight();
private:
	CShader*					m_pShaderCom = { nullptr };
	CModel*						m_pModelCom = { nullptr };
	_float						m_OpenTime = { 1.f };
	_bool                       m_bSet_Light = true;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	_wstring m_pModelName{};
	CInteractiveUI* m_InteractiveUI = { nullptr };
	CPlayer* m_pPlayer = { nullptr };
	_bool m_bState = { false }; // 애니설정을 한번만 해주는 변수
	_bool m_bOpen = { false };
	_bool Go_Move = { false };
	_uint m_iState = { 0 }; // 현재 문 상태 체크
	_bool m_bInterect = false;
	_uint m_DoorType{};
	_bool m_bGreenRight{};
	_float m_fSoundTimeSum{ 0.f };
	_bool m_bSoud{ false };
	_float4 m_RingColor {};
	_bool m_bEmissive{ false };
	_float4 m_fDoorEmissiveColor{};


public:
	static CDOOR* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END