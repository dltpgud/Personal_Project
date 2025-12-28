#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
class CProxyObject final : public CGameObject
{
public:
    struct MashInstanceDataCPU : CGameObject::GAMEOBJ_DESC
    {
        _wstring ModelTag{};
        _matrix WorldMatrix{};;
    };

private:
    CProxyObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CProxyObject(const CProxyObject& Prototype);
    virtual ~CProxyObject() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private :
       CModel* m_pModelCom = {nullptr};
 

public:
    static CProxyObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END