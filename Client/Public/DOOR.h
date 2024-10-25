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
	enum State {ClOSE, OPEN };
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
	virtual void Set_Model(const _wstring& protoModel) override;
	virtual CModel* Get_Model() override { return m_pModelCom; }
private:
	CShader*					m_pShaderCom = { nullptr };
	CModel*						m_pModelCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	_wstring m_pModelName{};
	CInteractiveUI* m_InteractiveUI = { nullptr };
	CPlayer* m_pPlayer = { nullptr };
	_bool m_bState = { false };
	_bool m_bOpen = { false };
	_uint m_iState = { 0 };
public:
	static CDOOR* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END