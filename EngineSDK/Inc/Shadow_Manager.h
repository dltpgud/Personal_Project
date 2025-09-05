#pragma once
#include "Base.h"
#include "Renderer.h"
#include "Light_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CShadow_Manager : public CBase
{
    DECLARE_SINGLETON(CShadow_Manager)

private:
    CShadow_Manager();
    virtual ~CShadow_Manager() = default;

public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Update();
    void Render();
    
    // 그림자 맵 생성
    void GenerateShadowMap();
    
    // 라이트 방향 설정
    void Set_LightDirection(_float3 vDirection);
    
    // 그림자 강도 설정
    void Set_ShadowIntensity(_float fIntensity) { m_fShadowIntensity = fIntensity; }
    _float Get_ShadowIntensity() const { return m_fShadowIntensity; }
    
    // 기존 Renderer의 Cascade 정보 가져오기
    const CRenderer::Cascade& GetCascade(_uint iIndex) const;
    
    // 그림자 쉐이더 상수 버퍼 업데이트
    void UpdateShadowConstants(ID3D11DeviceContext* pContext, class CShader* pShader);

private:
    _float3 m_vLightDirection;
    _float m_fShadowIntensity;
    
    // 기존 Renderer 참조
    class CRenderer* m_pRenderer;

private:
    ID3D11Device* m_pDevice = {nullptr};
    ID3D11DeviceContext* m_pContext = {nullptr};
    // 그림자 상수 버퍼
    struct SHADOW_CONSTANTS
    {
        _float4x4 LightViewMatrix[4];
        _float4x4 LightProjMatrix[4];
        _float4 CascadeSplits;
        _float3 LightDirection;
        _float ShadowIntensity;
    };
    
    SHADOW_CONSTANTS m_ShadowConstants;
    ID3D11Buffer* m_pShadowConstantBuffer;

public:
    static CShadow_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

END
