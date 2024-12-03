#pragma once
#include "Client_Defines.h"
#include "Skill.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Point;
END

BEGIN(Client)

class CTrail final : public CSkill
{
public:

public:

    struct TrailSegment {
        _float3 start;    // 시작점
        _float3 end;      // 끝점
        _float3 direction; // 트레일 방향
        _float alpha;       // 투명도
        _float lifeTime;    // 남은 수명
    };


    typedef struct CTrail_DESC : CSkill::Skill_DESC
    {
        const _uint* state = { nullptr };
        _vector pTagetPos{};
        _vector* NPos{};
    }CTrail_DESC;
private:
    CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CTrail(const CTrail& Prototype);
    virtual ~CTrail() = default;

public:
    /* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
    virtual HRESULT Initialize_Prototype() override;

    /* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
    /* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual void Dead_Rutine(_float fTimeDelta) override;

private:
    HRESULT Add_Components();
    HRESULT Bind_ShaderResources();
    HRESULT BIND_BULLET_TYPE();
private:
    _vector m_pTagetPos = {};
    _vector m_vDir{};
    _vector* m_NPos{};
    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer_Point* m_pVIBufferCom = { nullptr };
    _float2 m_pScale{};
    const _uint* m_pParentState = { nullptr };

    _float3 m_EndPos{};

    _float2 m_textureSize{ 1024.f,1024.f };
    _float2 m_frameSize{ 256.f,1024.f };
    _int m_framesPerRow{ 4 };
    _int m_currentFrame{ 0 };
    _float  m_fNTimeSum{ 0.f };

    _float3 m_fPos{};
    vector<TrailSegment> trailSegments;

public:
    static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END