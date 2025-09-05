#include "stdafx.h"
#include "CHEST.h"
#include "GameInstance.h"
#include "WeaPonIcon.h"
#include "InteractiveUI.h"
#include "Player.h"

CCHEST::CCHEST(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{pDevice, pContext}
{
}

CCHEST::CCHEST(const CCHEST& Prototype) : CGameObject{Prototype}
{
}

HRESULT CCHEST::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCHEST::Initialize(void* pArg)
{
    GAMEOBJ_DESC* pDesc = static_cast<GAMEOBJ_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    Set_Model(pDesc->ProtoName, pDesc->CuriLevelIndex);

    m_pWeaPonType = pDesc->Object_Type;

    m_iBoneIndex = m_pModelCom->Get_BoneIndex("Top_Base");

    m_InteractiveUI = static_cast<CInteractiveUI*>(m_pGameInstance->Find_Clone_UIObj(L"Interactive"));
    Safe_AddRef(m_InteractiveUI);

    Init_CallBakc();
    return S_OK;
}

void CCHEST::Priority_Update(_float fTimeDelta)
{
}

void CCHEST::Update(_float fTimeDelta)
{
    if (m_InteractiveUI->Get_Interactive(this) && (m_flags & HOVER))
    {
        m_pModelCom->Set_Animation(State::OPEN, false);
    }

    m_pModelCom->Play_Animation(fTimeDelta * 0.9f);

    if (m_flags & ICON)
    {
        XMMATRIX matrix = XMMatrixIdentity();
        matrix = XMMatrixScaling(0.f, 0.f, 0.f);
        m_pModelCom->Set_BoneUpdateMatrix(m_iBoneIndex, matrix);
    }

    __super::Update(fTimeDelta);
}

void CCHEST::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_SHADOW, this)))
        return;

    if (FAILED(m_pGameInstance->Add_Interctive(this)))
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CCHEST::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr())))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ShadowTransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(5)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CCHEST::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint j = 0; j < iNumMeshes; j++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, j, aiTextureType_DIFFUSE, 0,
                                                             "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, j, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(j);
    }

    return S_OK;
}

void CCHEST::Set_Model(const _wstring& protoModel, _uint ILevel)
{
    if (FAILED(__super::Add_Component(ILevel, protoModel, TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        MSG_BOX("Set_Model failed");
        return;
    }

    m_pModelCom->Set_Animation(State::IDLE, true);
}

HRESULT CCHEST::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
                                      TEXT("Com_ANIShader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CBounding_AABB::BOUND_AABB_DESC AABBDesc{};
    AABBDesc.vExtents = _float3(6.f, 1.f, 6.f);
    AABBDesc.vCenter = _float3(0.f, 1.f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
                                      TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom),
                                      &AABBDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCHEST::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

HRESULT CCHEST::Init_CallBakc()
{
    m_pModelCom->Callback(OPEN, 15,[this]()
                          {
                            CWeaPonIcon::CWeaPonIcon_DESC Desc;
                            Desc.WeaPonIndex = m_pWeaPonType;
                            Desc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
                            m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Layer_Map"), L"Prototype GameObject_WeaPonIcon", &Desc);
                            m_pGameInstance->Set_OpenUI(true, TEXT("WeaPon_Aim"));
                            m_pGameInstance->Set_OpenUI(false, TEXT("Interactive"), this);
                            m_flags &= ~HOVER;
                            m_flags |= ICON;
                          });

    m_pModelCom->Callback(OPEN, 0,[this](){
                              m_InteractiveUI->Set_OnwerPos(m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION));
                              m_pGameInstance->Set_OpenUI(true, TEXT("Interactive"), this);
                              m_pGameInstance->Play_Sound(L"ST_Chest_Open.ogg", nullptr, 1.f);
                          });


    m_pColliderCom->SetTriggerCallback(
        [this](CActor* other, _bool bColliding, _bool bPlayer)
        {
            if (bColliding && bPlayer)
            {
                if (!(m_flags & HOVER) && !(m_flags & ICON))
                {
                   m_InteractiveUI->Set_Text(L"상자 열기");
                   m_InteractiveUI->Set_OnwerPos(m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION));
                   m_pGameInstance->Set_OpenUI(true, TEXT("Interactive"), this);
                   m_pModelCom->Set_Animation(State::HOVDER, true);
                   m_flags |= HOVER;
                }
            }
            else if (bPlayer)
            {
                if ((m_flags & HOVER) && !(m_flags & ICON))
                {
                    m_pGameInstance->Set_OpenUI(true, TEXT("WeaPon_Aim"));
                    m_pGameInstance->Set_OpenUI(false, TEXT("Interactive"), this);
                   m_pModelCom->Set_Animation(State::IDLE, true);
                   m_flags &= ~HOVER;
                };
            }
        });

    return S_OK;
}

CCHEST* CCHEST::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCHEST* pInstance = new CCHEST(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMonster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCHEST::Clone(void* pArg)
{
    CCHEST* pInstance = new CCHEST(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CAniObj");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCHEST::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_InteractiveUI);
}
