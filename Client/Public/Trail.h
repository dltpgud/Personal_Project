#pragma once
#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Trail;
END

BEGIN(Client)

class CTrail final : public CGameObject
{
public:
    typedef struct CTrail_DESC : CGameObject::GAMEOBJ_DESC
    {
        _float3* fstartPoint{};
        _float3* fendPoint{}; 
        _int     iTrailSegments ;  // 部府 技弊刚飘 俺荐
        _float   fTrailLength; // 部府 辨捞
        _float   fTrailWidth ; // 部府 气
        _float4  fClolor[CSkill::COLOR::COLOR_END];
        _uint* pParantObject{};
    }CTrail_DESC;

private:
    CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CTrail(const CTrail& Prototype);
    virtual ~CTrail() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;


private:
    virtual HRESULT Add_Components() override;
    HRESULT Bind_ShaderResources();

private:
    CVIBuffer_Trail* m_pVIBufferCom = nullptr;
    CShader* m_pShaderCom = nullptr;
    CTexture* m_pTextureCom = nullptr;

    _float4  m_fClolor[CSkill::COLOR::COLOR_END];
    _float3* m_fstartPos{};
    _float3* m_fendPos{};
    _float2  m_textureSize{1024.f, 512.f};
    _float2  m_frameSize{256.f, 256.f};
    _int     m_framesPerRow{4};
    _int     m_currentFrame{0};
    _float   m_fTimeSum{};
    _int     m_iTrailSegments{};       // 部府 技弊刚飘 俺荐
    _float   m_fTrailLength{};     // 部府 辨捞
    _float   m_fTrailWidth{};        // 部府 气
    _uint* m_bState{};


public:
    static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END