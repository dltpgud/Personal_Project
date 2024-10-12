#include "stdafx.h"
#include "WeaPonIcon.h"
#include "GameInstance.h"
#include "WeaponUI.h"
CWeaPonIcon::CWeaPonIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{pDevice, pContext}
{
}

CWeaPonIcon::CWeaPonIcon(const CWeaPonIcon& Prototype) : CGameObject{Prototype}
{
}

HRESULT CWeaPonIcon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeaPonIcon::Initialize(void* pArg)
{
    m_DATA_TYPE = GAMEOBJ_DATA::DATA_NONANIMAPOBJ;
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;
    
    m_pTransformCom->Set_MoveSpeed(0.05f);
    m_moveTime = 1.f;
  
    return S_OK;
}

_int CWeaPonIcon::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
    {
        return OBJ_DEAD;
    }

    return OBJ_NOEVENT;
}

void CWeaPonIcon::Update(_float fTimeDelta)
{
    m_pTransformCom->Rotation_to_Player();


    if (m_moveTime > 0.f)
    {
        m_moveTime -= fTimeDelta;

        m_pTransformCom->Go_Up(fTimeDelta);

    }
    if (m_moveTime <= 0.f)
    {
        m_pTransformCom->Go_Down(fTimeDelta);


        _float Y = XMVectorGetY(m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION));
        if (Y <= m_fY)
        {
            m_moveTime = 1.f;
            m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, XMVectorSet(m_fX, m_fY, m_fZ, 1.f));
        }
    }
   
    if (m_pGameInstance->Get_DIKeyDown(DIK_I))
    {
        static_cast<CWeaponUI*>(m_pGameInstance->Get_UI(LEVEL_STATIC, CUI::UIID_PlayerWeaPon))
            ->Set_ScecondWeapon(m_weaPon);
        m_bDead = true;
    }
}

void CWeaPonIcon::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
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

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom[m_weaPon]->Render(j);
    }

    return S_OK;
}

void CWeaPonIcon::Set_WeaPone(const _uint& i)
{
    m_weaPon = i; 

}

HRESULT CWeaPonIcon::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    if (FAILED(__super::Add_Component(
        LEVEL_STATIC, TEXT("Proto Component ChestWeapon Model_AssaultRifle_NonAni"), TEXT("Com_Model0"),
        reinterpret_cast<CComponent**>(&m_pModelCom[0]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(
            LEVEL_STATIC, TEXT("Proto Component ChestWeapon Model_AssaultRifle_NonAni"), TEXT("Com_Model1"),
            reinterpret_cast<CComponent**>(&m_pModelCom[CWeapon::WeaPoneType::AssaultRifle]))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(
            LEVEL_STATIC, TEXT("Proto Component ChestWeapon Model_MissileGatling_NonAni"), TEXT("Com_Model2"),
            reinterpret_cast<CComponent**>(&m_pModelCom[CWeapon::WeaPoneType::MissileGatling]))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(
            LEVEL_STATIC, TEXT("Proto Component ChestWeapon Model_HeavyCrossbow_NonAni"), TEXT("Com_Model3"),
            reinterpret_cast<CComponent**>(&m_pModelCom[CWeapon::WeaPoneType::HeavyCrossbow]))))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeaPonIcon::Bind_ShaderResources()
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
}
