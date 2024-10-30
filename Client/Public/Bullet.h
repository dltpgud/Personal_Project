#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

END

BEGIN(Client)

class CBullet final : public CGameObject
{
public:
    typedef struct CBULLET_DESC : CGameObject::GAMEOBJ_DESC
    {
        _vector pTagetPos{};
        _vector vPos{};
        _vector vPlayerAt{};
    }CBULLET_DESC;
private:
    CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBullet(const CBullet& Prototype);
    virtual ~CBullet() = default;

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

private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();
    _vector m_pTagetPos = {};
    _vector m_vPos{};
    _vector m_vPlayerAt{};
public:
    static CBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END