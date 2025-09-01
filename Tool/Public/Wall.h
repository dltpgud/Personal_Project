#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;

END

BEGIN(Tool)
class CWALL final : public CGameObject
{
private:
    CWALL(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CWALL(const CWALL& Prototype);
    virtual ~CWALL() = default;

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
    virtual void Set_Model( const _wstring& protoModel, _uint ILevel) override;
    _wstring Get_ComPonentName(){ return m_wModel; }
    _tchar* Get_ProtoName();

private:
    CShader* m_pShaderCom = {nullptr};
    CModel* m_pModelCom = {nullptr};

private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();
    _wstring m_wModel;
    _tchar* m_Proto;
public:
    static CWALL* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END