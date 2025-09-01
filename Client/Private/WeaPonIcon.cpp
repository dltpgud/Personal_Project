#include "stdafx.h"
#include "WeaPonIcon.h"
#include "GameInstance.h"
#include "WeaponUI.h"
#include "InteractiveUI.h"
#include "Player.h"
CWeaPonIcon::CWeaPonIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartObject{pDevice, pContext}
{
}

CWeaPonIcon::CWeaPonIcon(const CWeaPonIcon& Prototype) : CPartObject{Prototype}
{
}

HRESULT CWeaPonIcon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeaPonIcon::Initialize(void* pArg)
{
    CWeaPonIcon_DESC* pDesc = static_cast<CWeaPonIcon_DESC*>(pArg); 
      m_weaPon = pDesc->WeaPonIndex;
      pDesc->fSpeedPerSec = 0.05f;
      
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Init_MSG()))
        return E_FAIL;

    if (FAILED(Init_CallBack()))
        return E_FAIL;

    m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSet(0.f, 1.f, 0.f, 1.f)); 

    return S_OK;
}

void CWeaPonIcon::Priority_Update(_float fTimeDelta)
{
}

void CWeaPonIcon::Update(_float fTimeDelta)
{
    if (true == m_InteractiveUI->Get_Interactive(this))
    {
        static_cast<CWeaponUI*>(m_pGameInstance->Find_Clone_UIObj(L"WeaponUI"))->Set_ScecondWeapon(m_weaPon);
        m_bDead = true;
        m_pGameInstance->Set_OpenUI(true, TEXT("WeaPon_Aim"));
        m_pGameInstance->Set_OpenUI(false, TEXT("Interactive"), this);
    }

    m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));
}

void CWeaPonIcon::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    Rotation(m_pGameInstance->Get_Player()->Get_Transform());

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;

    if (FAILED(m_pGameInstance->Add_Interctive(this)))
        return;
}

HRESULT CWeaPonIcon::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMesh = m_pModelCom[m_weaPon]->Get_NumMeshes();
    for (_uint j = 0; j < iNumMesh; j++)
    {
        if (FAILED(m_pModelCom[m_weaPon]->Bind_Material_ShaderResource(m_pShaderCom, j, aiTextureType_DIFFUSE, 0,
                                                                       "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        m_pModelCom[m_weaPon]->Render(j);
    }

    return S_OK;
}

HRESULT CWeaPonIcon::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Proto Component ChestWeapon Model_AssaultRifle_NonAni"), TEXT("Com_Model1"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom[CWeapon::WeaPoneType::AssaultRifle]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Proto Component ChestWeapon Model_MissileGatling_NonAni"), TEXT("Com_Model2"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom[CWeapon::WeaPoneType::MissileGatling]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Proto Component ChestWeapon Model_HeavyCrossbow_NonAni"), TEXT("Com_Model3"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom[CWeapon::WeaPoneType::HeavyCrossbow]))))
        return E_FAIL;

    CBounding_AABB::BOUND_AABB_DESC AABBDesc{};
    AABBDesc.vExtents = _float3(6.f, 1.f, 6.f);
    AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"),
                                      reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeaPonIcon::Bind_ShaderResources()
{
    if (FAILED( m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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

HRESULT CWeaPonIcon::Init_CallBack()
{
    m_pColliderCom->SetTriggerCallback(
        [this](CActor* other, _bool bColliding,_bool bPlayer)
        {
            if (bColliding && bPlayer)
            {
                if (false == m_bDead)
                {
                    m_InteractiveUI->Set_Text(m_pWeaPonNumName);
                    m_pGameInstance->Set_OpenUI(true, TEXT("Interactive"), this);
                    m_InteractiveUI->Set_OnwerPos(XMVectorSet(m_WorldMatrix._41,m_WorldMatrix._42,m_WorldMatrix._43,m_WorldMatrix._44));
                }
            }
            else if (bPlayer)
            {
                m_pGameInstance->Set_OpenUI(true, TEXT("WeaPon_Aim"));
                m_pGameInstance->Set_OpenUI(false, TEXT("Interactive"), this);
            }
        });

    return S_OK;
}

HRESULT CWeaPonIcon::Init_MSG()
{
    m_InteractiveUI = static_cast<CInteractiveUI*>(m_pGameInstance->Find_Clone_UIObj(L"Interactive"));
    Safe_AddRef(m_InteractiveUI);

    switch (m_weaPon)
    {
      case CWeapon::WeaPoneType::HendGun: m_pWeaPonNumName = L"HendGun ÀåÂø"; break;
      case CWeapon::WeaPoneType::AssaultRifle: m_pWeaPonNumName = L"Assault Rifle ÀåÂø"; break;
      case CWeapon::WeaPoneType::MissileGatling: m_pWeaPonNumName = L"Missile Gatling ÀåÂø"; break;
      case CWeapon::WeaPoneType::HeavyCrossbow: m_pWeaPonNumName = L"Heavy Crossbow ÀåÂø"; break;
    }

    return S_OK;
}

void CWeaPonIcon::Rotation(CTransform* Transform)
{
  _matrix met = XMMatrixLookAtLH(XMLoadFloat4x4(&m_WorldMatrix).r[3], Transform->Get_TRANSFORM(CTransform::T_POSITION),XMVectorSet(0.f, 1.f, 0.f, 0.f));

  _matrix Wmet = XMMatrixInverse(nullptr, met);

  _float4x4 wmet{};

  XMStoreFloat4x4(&wmet, Wmet);
  XMStoreFloat4((_float4*)&m_WorldMatrix.m[0][0], XMVectorSet(wmet._11, 0.f, wmet._13, wmet._14));
  XMStoreFloat4((_float4*)&m_WorldMatrix.m[1][0], XMVectorSet(0.f, 1.f, 0.f, wmet._24));
  XMStoreFloat4((_float4*)&m_WorldMatrix.m[2][0], XMVectorSet(wmet._31, 0.f, wmet._33, wmet._34));
}

CWeaPonIcon* CWeaPonIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWeaPonIcon* pInstance = new CWeaPonIcon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CWeaPonIcon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeaPonIcon::Clone(void* pArg)
{
    CWeaPonIcon* pInstance = new CWeaPonIcon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CWeaPonIcon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeaPonIcon::Free()
{
    __super::Free();

    for (_uint i = 0; i < CWeapon::WeaPoneType_END; i++) { Safe_Release(m_pModelCom[i]); }
    Safe_Release(m_pShaderCom);
    Safe_Release(m_InteractiveUI);
}
