#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CWall final : public CGameObject
{

private:
    CWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CWall(const CWall& Prototype);
    virtual ~CWall() = default;

public:
    /* ���������� ȣ�� : ������ �ʿ��� ����� ���ſ� �۾����� �����Ѵ�.(��Ŷ, ���� �����) */
    virtual HRESULT Initialize_Prototype() override;

    /* ��Ŷ�̳� ���� ������� ���ؼ� �޾ƿ��� ���ϴ� �����鵵 �и��� �����Ѵ�. */
    /* �������� �����ϴ� �ʴ� �߰����� �ʱ�ȭ�� �ʿ��� ��� ȣ���Ѥ���. */
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual HRESULT Render_Shadow() override;
    virtual void Set_Model(const _wstring& protoModel, _uint ILevel) override;
    virtual CModel* Get_Model() override { return m_pModelCom; }
   

private:
    CShader* m_pShaderCom = { nullptr };
    CModel* m_pModelCom = { nullptr };
    _bool m_bEmissive{ false };
    _float4 m_fDoorEmissiveColor{};
    _wstring m_pModelName{};


private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();

public:
    static CWall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END