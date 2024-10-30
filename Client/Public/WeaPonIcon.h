#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Weapon.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
    class CWeaPonIcon final : public CGameObject
{
private:
    CWeaPonIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CWeaPonIcon(const CWeaPonIcon& Prototype);
    virtual ~CWeaPonIcon() = default;

public:
    /* ���������� ȣ�� : ������ �ʿ��� ����� ���ſ� �۾����� �����Ѵ�.(��Ŷ, ���� �����) */
    virtual HRESULT Initialize_Prototype() override;

    /* ��Ŷ�̳� ���� ������� ���ؼ� �޾ƿ��� ���ϴ� �����鵵 �и��� �����Ѵ�. */
    /* �������� �����ϴ� �ʴ� �߰����� �ʱ�ȭ�� �ʿ��� ��� ȣ���Ѥ���. */
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override; 
    virtual HRESULT Render() override;
    void Set_WeaPone(const _uint& i);
    void Set_PosSave(_float  x, _float y , _float z) { m_fY = y, m_fX = x , m_fZ = z; }
private:
    CShader* m_pShaderCom = {nullptr};
    CModel* m_pModelCom[CWeapon::WeaPoneType_END]{};
    _uint  m_weaPon{};
    _float m_moveTime{ 0.f };
    _float m_fX{ 0.f };
    _float m_fY{ 0.f };
    _float m_fZ{ 0.f };
    _tchar* m_pWeaPonNumName = {};
    class CInteractiveUI* m_InteractiveUI = {nullptr};
    class CPlayer* m_pPlayer = { nullptr };
private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();

public:
    static CWeaPonIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END