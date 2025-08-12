#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
    class CNonAni final : public CGameObject
{
private:
    CNonAni(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CNonAni(const CNonAni& Prototype);
    virtual ~CNonAni() = default;

public:
   
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render_Shadow() override;
    virtual HRESULT Render() override;
    virtual void Set_Model(const _wstring& protoModel, _uint ILevel) override;
    virtual CModel* Get_Model() override { return m_pModelCom; }
    virtual void Set_InstaceBuffer(const vector<_matrix>& worldmat);


private:
    CShader* m_pShaderCom = {nullptr};
    CModel* m_pModelCom = {nullptr};
    _wstring m_wModel{};
    _uint m_iPass{ 0 };
private:

    HRESULT Bind_ShaderResources();

public:
    static CNonAni* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END