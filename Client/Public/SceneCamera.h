#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)
class CSceneCamera final: public CCamera
{
 public:
	typedef struct CSceneCamera_DESC : public CCamera::CAMERA_DESC
	{
		_vector vStopPos{};
	}CSceneCamera_DESC;
private:
	CSceneCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSceneCamera(const CSceneCamera& Prototype);
	virtual ~CSceneCamera() = default;

public:

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	_bool Get_IsCamEnd() {
		return m_bIsCamEnd;
	}
private:
	_vector	m_vStopPos{};
	_float m_fRunTime = 0.f;
	_float m_TimeSum = { 0.f };
	_bool m_bIsCamEnd = { false };
public:
	static CSceneCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
