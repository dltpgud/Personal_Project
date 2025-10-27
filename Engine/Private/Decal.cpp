#include "Decal.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"

CDecal::CDecal() : m_pGameInstance{CGameInstance::GetInstance()}
{

    Safe_AddRef(m_pGameInstance);
}

CDecal::CDecal(const CDecal& Prototype) : m_pGameInstance{Prototype.m_pGameInstance}
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CDecal::Initialize_Proto()
{
    return S_OK;
}

HRESULT CDecal::Initialize(void* pArg)
{
    if (m_LifeState == OBJ_POOL)
        m_LifeState = OBJ_NOEVENT;
    
    DECAL_DESC* pDecalDesc = static_cast<DECAL_DESC*>(pArg);
    m_fLifeTime = pDecalDesc->fLifeTime;
    m_iDecalType = pDecalDesc->iType;
    m_bNormal = pDecalDesc->bNormal;
    m_fDecalTime = 0.f;
    m_iTexIndex = pDecalDesc->iTexIndex;
    m_fDecalSize = {pDecalDesc->fSize, pDecalDesc->fSize, pDecalDesc->fDepth};
    m_fDeltaScaling = pDecalDesc->DeltaScaling; 
    m_ProtoKey = pDecalDesc->Key;
    


    if (m_iDecalType == DECAL_DESC::TYPE_SSD)
    {
        m_fDecalSize = {pDecalDesc->fSize, pDecalDesc->fSize, pDecalDesc->fSize};
        XMStoreFloat3(&m_fDecalDir, pDecalDesc->vDir);
    }
    else
    {
        m_fDecalSize = {pDecalDesc->fSize, pDecalDesc->fSize, pDecalDesc->fDepth};
        XMStoreFloat3(&m_fDecalPos, pDecalDesc->vPos);
        XMStoreFloat3(&m_fDecalDir, pDecalDesc->vNormal); //Box에서는 이렇게 저장하자
        _vector n = XMVector3Normalize(pDecalDesc->vNormal);
        _vector t = XMVector3Normalize(XMVector3Orthogonal(n));
        _vector b = XMVector3Normalize(XMVector3Cross(n, t));
        _matrix TBN = _matrix(t, b, n, XMVectorSet(0, 0, 0, 1));
        _matrix S = XMMatrixScaling(pDecalDesc->fSize, pDecalDesc->fSize, pDecalDesc->fDepth);
        _matrix T = XMMatrixTranslationFromVector(pDecalDesc->vPos + n * 0.001f);
        _matrix world = S * TBN * T;
        _matrix invWorld = XMMatrixInverse(nullptr, world);
                XMStoreFloat4x4(&m_WorldMatInv, invWorld);
    }
    return S_OK;
}

void CDecal::Update(_float fTimeDelta)
{
    m_fDecalTime += fTimeDelta;
    m_fLifeTime -= fTimeDelta;

    if (m_fLifeTime <= 0.f)
        m_LifeState = OBJ_POOL;



    if (m_iDecalType == DECAL_DESC::TYPE_BOX && m_fDeltaScaling !=0)
    {
       m_fDecalSize.x += m_fDeltaScaling * fTimeDelta;
       m_fDecalSize.y += m_fDeltaScaling * fTimeDelta;
       m_fDecalSize.z += m_fDeltaScaling * fTimeDelta;
       _vector n = XMVector3Normalize(XMLoadFloat3(&m_fDecalDir));
       _vector t = XMVector3Normalize(XMVector3Orthogonal(n));
       _vector b = XMVector3Normalize(XMVector3Cross(n, t));
       _matrix TBN = _matrix(t, b, n, XMVectorSet(0, 0, 0, 1));
       _matrix S = XMMatrixScaling(m_fDecalSize.x, m_fDecalSize.y, m_fDecalSize.z);
       _matrix T = XMMatrixTranslationFromVector(XMVectorSetW( XMLoadFloat3(&m_fDecalPos),1.f) + n * 0.001f);
       _matrix world = S * TBN * T;
       _matrix invWorld = XMMatrixInverse(nullptr, world);
       XMStoreFloat4x4(&m_WorldMatInv, invWorld);
       
    }
}

HRESULT CDecal::Render(CShader* pShader)
{
    return S_OK;
}

CDecal* CDecal::Create()
{
    CDecal* pInstance = new CDecal();

    if (FAILED(pInstance->Initialize_Proto()))
    {
        MSG_BOX("Failed to Created : CDecal");
        Safe_Release(pInstance);
    }

    return pInstance;
}
CDecal* CDecal::Clone(void* pArg)
{
    CDecal* pInstance = new CDecal(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CDecal");
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CDecal::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}
