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
    enum DoorType{ STAGE, ITEM, BOSS };
	enum State {OPEN, ClOSE};
	enum State2 { OPEN2, IDLE,ClOSE2};
private:
	CDOOR(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDOOR(const CDOOR& Prototype);
	virtual ~CDOOR() = default;

public:

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
        virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;	
	virtual HRESULT Render_Shadow() override;
	virtual void Set_Model(const _wstring& protoModel, _uint ILevel) override;

	HRESULT Add_StageDoorLight();
	HRESULT Add_BossDoorLight();

private:
	CShader*					m_pShaderCom = { nullptr };
	CModel*						m_pModelCom = { nullptr };
	_float						m_OpenTime = { 1.f };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	DoorType m_iDoorType{};
	CInteractiveUI* m_InteractiveUI = { nullptr };
	_bool m_bOpen = { false };
    _bool m_bSoud = {false};
	_uint m_iState = { 0 }; // 현재 문 상태 체크
	_bool m_bInterect = false;

	_uint m_ChangeLevelDoor{};
	_float4 m_RimColor{};
	_bool m_bEmissive{ false };
	_float4 m_fDoorEmissiveColor{};


public:
	static CDOOR* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END