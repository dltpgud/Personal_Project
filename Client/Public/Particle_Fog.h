#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Point;
END

BEGIN(Client)

class CParticle_Fog final : public CGameObject
{
public: 
	typedef struct CParticle_Fog_Desc : CGameObject::GAMEOBJ_DESC
	{
		 _matrix pParentMatrix{};
	} CCParticle_FogDESC;


private:
    CParticle_Fog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CParticle_Fog(const CParticle_Fog& Prototype);
    virtual ~CParticle_Fog() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
        virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;	

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Particle_Point*	m_pVIBufferCom = { nullptr };

private:
  virtual HRESULT Add_Components() override;
	HRESULT Bind_ShaderResources();
    _float4x4 m_NewWordMatrix{};
    _matrix m_pParentMatrix{};

	_float2 m_textureSize{1024.f,512.f};
	_float2 m_frameSize{256.f,256.f};
	_int m_framesPerRow{4};
	_int m_currentFrame{0};
	

	_float m_fTimeSum{};

public:
    static CParticle_Fog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END