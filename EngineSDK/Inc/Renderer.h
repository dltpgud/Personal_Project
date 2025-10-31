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
        RG_NONLIGHT,
        RG_UI,
        RG_END
    };
	enum SIZE {SIZE_ORIGINAL, SIZE_DOWN_2, SIZE_DOWN_4, SIZE_DOWN_8, SIZE_SHADOW, SIZE_END};

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize(_uint iWinSizeX, _uint iWinSizeY);
	HRESULT Add_RenderGameObject(RENDERGROUP eRenderGroup, class CGameObject* pRenderGameObject);
	HRESULT Add_DebugComponents(class CComponent* pComponent);
	HRESULT Draw();

private:
        HRESULT Initialize_SizeViewPort();
        HRESULT Initialize_RT();
        HRESULT Initialize_MRT();
        HRESULT Add_Components();
     
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	ID3D11DepthStencilView* m_pLightDepthStencilView = { nullptr };   

	 class CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	 class CShader* m_pShader = { nullptr };
     class CShader* m_pDecalShader = {nullptr};
      class CSSAO_ComputeShader* m_pCS_SSAO = nullptr;
	 class CGameInstance* m_pGameInstance = { nullptr };
  
     _float4x4 m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};

private:
     _uint m_iWinSizeX{ 0 };
     _uint m_iWinSizeY{ 0 };
     D3D11_VIEWPORT m_ViewPortDescs[SIZE_END]{};
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
    HRESULT Render_Decal();	
	HRESULT Render_Effect();
	HRESULT Render_Lights(); /* 빛들의 연산결과를 MRT_LightAcc에 소속된 타겟들에게 그려준다. */
    HRESULT Render_LightsCombine();
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