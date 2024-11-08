#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Terrain;
class CNavigation;
END

BEGIN(Client)
class CTerrain : public CGameObject
{
    enum Terrain_TYPE { TYPE_MAIN};

private:
    CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CTerrain(const CTerrain& Prototype);
    virtual ~CTerrain() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;

    virtual HRESULT Render() override;

    virtual void Set_Model(const _wstring& protoModel, _uint ILevel) override;
    virtual void Set_Buffer(_uint x, _uint y)override;
    CVIBuffer_Terrain* Get_buffer()
    {
        return m_pVIBufferCom;
    }
    virtual _float3* Get_VtxPos();

    _uint Get_SizeX()
    {
        return m_pSize[0];
    }
    _uint Get_SizeY()
    {
        return m_pSize[1];
    }
    virtual void Set_Scalra_uint(_uint scalra);
    virtual void Set_Scalra_float(_float scalra);
private:
    CTexture* m_pTextureCom = {nullptr};
    CShader* m_pShaderCom = {nullptr};
    CVIBuffer_Terrain* m_pVIBufferCom = {nullptr};
    CNavigation* m_pNavigationCom = {nullptr};
    _uint m_pSize[2]{};
    _bool m_bMain = {false};  // 이건 타일 수정할 떄 꼭 하자
    _uint m_bFire{0};
    _float m_iUVoffset{ 0.f };
    _float m_fTimeSum{ 0.f };

private:
    HRESULT Add_Components();

public:
    static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END
