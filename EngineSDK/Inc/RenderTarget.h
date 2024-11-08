#pragma once

#include "Base.h"

/* 렌더타겟하나 그 자체를 의미하는 클래스이다.  */

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const {
		return m_pRTV;
	}

public:
	HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	void Clear();

#ifdef _DEBUG
public:

	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
	ID3D11Device*							m_pDevice = { nullptr };
	ID3D11DeviceContext*					m_pContext = { nullptr };

private:
	ID3D11Texture2D*						m_pTexture2D = { nullptr };
	ID3D11ShaderResourceView*				m_pSRV = { nullptr };
	ID3D11RenderTargetView*					m_pRTV = { nullptr };
	_float4									m_vClearColor = {};

#ifdef _DEBUG
private:
	_float4x4							m_WorldMatrix = {}; // 직교 투영을 위한 월드 행렬

#endif



public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	virtual void Free() override;
};

END