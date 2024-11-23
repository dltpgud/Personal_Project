#pragma once

#include "Base.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
private:
	CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);	
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSView, _bool isClear = true);
	HRESULT End_MRT(const _wstring& strMRTTag);
	HRESULT Bind_SRV(class CShader* pShader, const _char* pConstantName, const _wstring& strTargetTag);
	HRESULT Copy_Resource(const _wstring& strTargetTag, ID3D11Texture2D* pOut);
#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

	ID3D11RenderTargetView*			m_pBackBufferView = { nullptr };
	ID3D11DepthStencilView*			m_pDSV = { nullptr };

private:
	map<const _wstring, class CRenderTarget*>			m_RenderTargets;

	/* ��ġ�� "���ÿ� ���ε�"�Ǿ��ϴ� Ÿ�ٵ��� �̸� ��� �׷�ȭ���ѵд�. */
	map<const _wstring, vector<class CRenderTarget*>>		m_MRTs;   // ��Ƽ���� Ÿ��.

private:
	class CRenderTarget* Find_RenderTarget(const _wstring& strTargetTag);
	vector<class CRenderTarget*>* Find_MRT(const _wstring& strMRTTag);

public:
	static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END