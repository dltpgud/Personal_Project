#include "Target_Manager.h"
#include "RenderTarget.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"

CTarget_Manager::CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTarget_Manager::Initialize()
{
	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	if (nullptr != Find_RenderTarget(strTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext,iWidth, iHeight, ePixelFormat, vClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	vector<CRenderTarget*>* pMRTs = Find_MRT(strMRTTag);

	if (nullptr == pMRTs)
	{
		vector<CRenderTarget*>		RenderTargets;

		RenderTargets.reserve(8);

		RenderTargets.push_back(pRenderTarget);

		m_MRTs.emplace(strMRTTag, RenderTargets);
	}
	else
		pMRTs->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSView, _bool isClear)
{
	m_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDSV);

	vector<CRenderTarget*>* pMRTs = Find_MRT(strMRTTag);
	if (nullptr == pMRTs)
		return E_FAIL;

	ID3D11RenderTargetView* RTVs[8] = { nullptr };

	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
nullptr
	};

	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);


	size_t			iNumRenderTargets = pMRTs->size();

	for (size_t i = 0; i < iNumRenderTargets; i++)
	{
		if (true == isClear)
		(*pMRTs)[i]->Clear();
		RTVs[i] = (*pMRTs)[i]->Get_RTV();
	}

	m_pContext->OMSetRenderTargets(static_cast<_uint>(iNumRenderTargets), RTVs, nullptr != pDSView ? pDSView : m_pDSV);

	return S_OK;
}
HRESULT CTarget_Manager::End_MRT(const _wstring& strMRTTag)
{
	ID3D11RenderTargetView* pRTVs[] = {
		m_pBackBufferView,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
	};

	m_pContext->OMSetRenderTargets(8, pRTVs, m_pDSV);

	Safe_Release(m_pBackBufferView);
	Safe_Release(m_pDSV);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_SRV(CShader* pShader, const _char* pConstantName, const _wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResource(pShader, pConstantName);


}

HRESULT CTarget_Manager::Copy_Resource(const _wstring& strTargetTag, ID3D11Texture2D* pOut)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Copy_Resource(pOut);
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CTarget_Manager::Render_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer)
{
	vector<CRenderTarget*>* pRenderTargets = Find_MRT(strMRTTag);
	if (nullptr == pRenderTargets)
		return E_FAIL;

	for (auto& pRenderTarget : *pRenderTargets)
		pRenderTarget->Render(pShader, pVIBuffer);

	return S_OK;
}

#endif

CRenderTarget* CTarget_Manager::Find_RenderTarget(const _wstring& strTargetTag)
{
	auto	iter = m_RenderTargets.find(strTargetTag);

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

vector<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _wstring& strMRTTag)
{
	auto	iter = m_MRTs.find(strMRTTag);

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}


CTarget_Manager* CTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTarget_Manager* pInstance = new CTarget_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CTarget_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTarget_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);

		Pair.second.clear();
	}
	m_MRTs.clear();


	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);
	m_RenderTargets.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
