#include "stdafx.h"
#include "chest.h"

#include "GameInstance.h"

CChest::CChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{pDevice, pContext}
{
}

CChest::CChest(const CChest& Prototype) : CGameObject{Prototype}
{
}

HRESULT CChest::Initialize_Prototype()
{
    /* ��Ŷ, �����Ԥ������ ���� �ʱ�ȭ. */

    return S_OK;
}

HRESULT CChest::Initialize(void* pArg)
{
    GAMEOBJ_DESC* pDesc = static_cast<GAMEOBJ_DESC*>(pArg);
    m_DATA_TYPE = pDesc->DATA_TYPE;

    size_t iLen = wcslen(pDesc->ProtoName) + 1;
    m_Proto = new wchar_t[iLen];
    lstrcpyW(m_Proto, pDesc->ProtoName);

    /* �߰������� �ʱ�ȭ�� �ʿ��ϴٸ� �������ش�. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;




    return S_OK;
}

_int CChest::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

    if (m_pGameInstance->Get_DIKeyDown(DIK_K))
        m_istate++;

    m_pModelCom->Set_Animation(m_istate, false);
    __super::Priority_Update(fTimeDelta);
    return OBJ_NOEVENT;
}

void CChest::Update(_float fTimeDelta)
{
   
    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        _uint iData = 10;
    __super::Update(fTimeDelta);
}

void CChest::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
    __super::Late_Update(fTimeDelta);
}

HRESULT CChest::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
                                                             "g_DiffuseTexture")))
            return E_FAIL;

        /*�ִϿ� �߰�*/
        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }
    __super::Render();

    return S_OK;
}

void CChest::Set_Model(const _wstring& protoModel, _uint ILevel)
{
    m_wModel = protoModel;
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, protoModel, TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
        return;
    m_istate = 0;
    m_pModelCom->Set_Animation(m_istate, true);



    CBounding_OBB::BOUND_OBB_DESC		OBBDesc{};

    OBBDesc.vRotation = { 0.f,0.f,0.f };
    OBBDesc.vExtents = _float3(0.5f, 0.75f, 0.5f);
    OBBDesc.vCenter = _float3(0.f, 0.5f, 0.f);
    //AABBDesc.vExtents = _float3(0.5f, 0.75f, 0.5f);
    //AABBDesc.vCenter = _float3(0.f, 0.5f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return;

}

_tchar* CChest::Get_ProtoName()
{
    return m_Proto;
}

_float CChest::check_BoxDist(_vector RayPos, _vector RayDir)
{
    _matrix matWorld = m_pTransformCom->Get_WorldMatrix_Inverse();

    _vector CurRayPos = XMVector3TransformCoord(RayPos, matWorld);
    _vector CurRayDir = XMVector3TransformNormal(RayDir, matWorld);
    CurRayDir = XMVector3Normalize(CurRayDir);


    //RayDir = XMVector3Normalize(RayDir);
    _float Dist{};
    if (m_pColliderCom->RayIntersects(RayPos, RayDir, Dist))
    {
        return Dist;
    }

    return _float(0xffff);
}

HRESULT CChest::Add_Components()
{
    /* ��������� ���� ������ �ϰԵǸ� */
    /* 1. ���� �� ������Ʈ�� �̿��ϰ����� �� ���� �˻��� �ʿ���� Ư�� ��������� �ٷ� ����� �̿��ϸ� �ȴ�. */
    /* 2. �ٸ� ��ü�� �� ������Ʈ�� �˻��ϰ��� �Ҷ� ����ġ���̽��� �̳� �þ�� ��Ȳ. */

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CChest::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(
            m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(
            m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CChest* CChest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CChest* pInstance = new CChest(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CChest");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CChest::Clone(void* pArg)
{
    CChest* pInstance = new CChest(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CMonster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChest::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);

    if (m_bClone) 
        Safe_Delete_Array(m_Proto);
     
    
}
