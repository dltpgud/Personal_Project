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
public:
    struct CTerrain_DESC : CGameObject::GAMEOBJ_DESC
    {
        _int Buffer[2];
        _int   ilava{};
        _bool  IsMain{};
        _float flavaOffset{};
    };

private:
    CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CTerrain(const CTerrain& Prototype);
    virtual ~CTerrain() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;

    virtual HRESULT Render() override;
    virtual HRESULT Render_Shadow() override;
    virtual void Set_Model(const _wstring& protoModel, _uint ILevel) override;
    void Set_Buffer(_int BufferX, _int BufferY);
    _bool isPowerOfTwoPlusOne(_int num)
    {
        if (num <= 1)
            return false; 
        int x = num - 1;  // num - 1이 2의 거듭제곱인지 확인
        if((x & (x - 1)) == 0)
            return true;

        return false;
    }

private:
    CTexture* m_pTextureCom = {nullptr};
    CShader* m_pShaderCom = {nullptr};
    CVIBuffer_Terrain* m_pVIBufferCom = {nullptr};
    CNavigation* m_pNavigationCom = {nullptr};
    _uint m_pSize[2]{};
    _bool m_bMain = {false};
    _uint m_iFire{0};
    _float m_fUVoffset{ 0.f };
    _float m_fTimeSum{ 0.f };

private:
    HRESULT Add_Components(void* pArg);

public:
    static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END
