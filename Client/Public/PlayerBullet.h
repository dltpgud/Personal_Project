#pragma once
#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CPlayerBullet final : public CSkill
{
public:
    typedef struct CPlayerBullet_DESC : CSkill::Skill_DESC
    {
        _vector pTagetPos{};
        _vector vPlayerAt{};
    }CPlayerBullet_DESC;
private:
    CPlayerBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CPlayerBullet(const CPlayerBullet& Prototype);
    virtual ~CPlayerBullet() = default;

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
private:
    _vector m_pTagetPos = {};
    _vector m_vPlayerAt{};
    _vector m_vDir{};


private:
    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
public:
    static CPlayerBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END