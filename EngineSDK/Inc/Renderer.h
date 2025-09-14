#pragma once
#include "Base.h"

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
    enum RENDERGROUP
    {
        RG_PRIORITY,
        RG_SHADOW,
		RG_NONBLEND,
        RG_BLOOM,
        RG_NONLIGHT,
        RG_UI,
        RG_END
    };
	enum SIZE {SIZE_ORIGINAL, SIZE_DOWN_4, SIZE_DOWN_44, SIZE_DOWN_444, SIZE_SHADOW, SIZE_SHADOWBG, SIZE_END};

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize(_uint iWinSizeX, _uint iWinSizeY);
	HRESULT Add_RenderGameObject(RENDERGROUP eRenderGroup, class CGameObject* pRenderGameObject);
	HRESULT Add_DebugComponents(class CComponent* pComponent);
	HRESULT Draw();
	void Initialize_SizeViewPort();
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	ID3D11DepthStencilView* m_pLightDepthStencilView = { nullptr };    
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	class CShader* m_pShader = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
  
    _float4x4 m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};

private:
 // 기본 뷰포트 사이즈 저장 변수
_uint m_iWinSizeX{ 0 };
_uint m_iWinSizeY{ 0 };
 // 사이즈별 뷰포트들
 D3D11_VIEWPORT m_ViewPortDescs[SIZE_END]{};
// 뷰포트 사이즈 마다 샘플링할 텍스쳐 쿠드들 좌표
_float m_fdX[SIZE_END]{}; 
_float m_fdY[SIZE_END]{};

private:
	list<class CGameObject*>	m_RenderGameObjects[RG_END];
	list<class CComponent*>		m_DebugComponents;

private:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();;
	HRESULT Render_NonBlend(); /* MRT_GameObjects에 소속된 타겟들에게 객체들의 특정 정보(Diffuse + Normal)를 기록해준다. */
	HRESULT Render_Bloom();

	HRESULT Render_Lights(); /* 빛들의 연산결과를 MRT_LightAcc에 소속된 타겟들에게 그려준다. */
	HRESULT Render_Final();
	HRESULT Render_NonLight();
	HRESULT Render_UI();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
#endif


public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;
};

END