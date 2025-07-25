#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Weapon.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
    class CWeaPonIcon final : public CPartObject
{
public:
    typedef struct CWeaPonIcon_DESC : CPartObject::PARTOBJECT_DESC
    {
        _int WeaPonIndex;
    } WeaPonIcon_DESC;

private:
    CWeaPonIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CWeaPonIcon(const CWeaPonIcon& Prototype);
    virtual ~CWeaPonIcon() = default;

public:

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override; 
    virtual HRESULT Render() override;
private:
    CShader* m_pShaderCom = {nullptr};
    CModel* m_pModelCom[CWeapon::WeaPoneType_END]{};
    _uint  m_weaPon{};
    _tchar* m_pWeaPonNumName = {};
    _bool m_bIsColl{false};
    class CInteractiveUI* m_InteractiveUI = {nullptr};
private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();

public:
    static CWeaPonIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END