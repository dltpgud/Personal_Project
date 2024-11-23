#pragma once
#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Point;
END

BEGIN(Client)

class CBullet final : public CSkill
{
public:

public:
    typedef struct CBULLET_DESC : CSkill::Skill_DESC
    {
        _float* Fall_Y{};
 
        _vector pTagetPos{};

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
    HRESULT BIND_BULLET_TYPE();
private:
    _vector m_pTagetPos = {};
    _vector m_vPlayerAt{};
    _vector m_vDir{};
    _float* m_fFall_Y = { nullptr };

    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer_Point* m_pVIBufferCom = { nullptr };

    _float2 m_pScale{};
public:
    static CBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END