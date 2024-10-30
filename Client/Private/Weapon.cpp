#include "stdafx.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Player.h"
#include "UI.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartObject{pDevice, pContext}
{
}

CWeapon::CWeapon(const CWeapon& Prototype) : CPartObject{Prototype}
{
}

HRESULT CWeapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
    WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

    m_pParentState = pDesc->pParentState;
    m_pSocketMatrix = pDesc->pSocketMatrix;
    m_pType = pDesc->pType;
    m_pWeapon = 0;
    m_iCurMotion = Idle;

    m_iMaxBullet[CWeapon::HendGun] = 15;
    m_iMaxBullet[CWeapon::AssaultRifle] = 30;
    m_iMaxBullet[CWeapon::MissileGatling] = 20;
    m_iMaxBullet[CWeapon::HeavyCrossbow] = 7;

    m_iFirstBullet[CWeapon::HendGun] = 15;
    m_iFirstBullet[CWeapon::AssaultRifle] = 30;
    m_iFirstBullet[CWeapon::MissileGatling] = 20;
    m_iFirstBullet[CWeapon::HeavyCrossbow] = 7;
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scaling(0.01f, 0.01f, 0.01f);


       
    return S_OK;
}

_int CWeapon::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;




    return OBJ_NOEVENT;
}

void CWeapon::Update(_float fTimeDelta)
{

    *m_pType == CPlayer::T00 ? Type0_Update(fTimeDelta) : Type2_Update(fTimeDelta);

        m_pGameInstance->UI_shaking(CUI::UIID_PlayerHP, fTimeDelta);
    m_pGameInstance->UI_shaking(CUI::UIID_PlayerWeaPon, fTimeDelta);
    m_pGameInstance->UI_shaking(CUI::UIID_PlayerWeaPon_Aim, fTimeDelta);


}

void CWeapon::Late_Update(_float fTimeDelta)
{
    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; i++)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]); // 항등으로 만들어서 넣겠다..

    XMStoreFloat4x4(&m_WorldMatrix,
        m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
}

HRESULT CWeapon::Render()
{  
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom[m_pWeapon]->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom[m_pWeapon]->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
                                                                        "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pModelCom[m_pWeapon]->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom[m_pWeapon]->Render(i);
    }

    return S_OK;
}

void CWeapon::Type0_Update(_float fTimeDelta)
{
    _bool bMotionChange = {false}, bLoop = {false};
    if (*m_pParentState == CPlayer::STATE_HENDGUN_RELOAD && m_iCurMotion != Reload)
    {
        m_iBullet = CWeapon::HendGun;
        m_iMaxBullet[m_iBullet] = 15;
        m_iCurMotion = Reload;
        bMotionChange = true;
        bLoop = false;
    }

    if (*m_pParentState == CPlayer::STATE_HENDGUN_SHOOT && m_iCurMotion != Shoot)
    {
        m_iMaxBullet[m_iBullet] -= 1;

        if (0 >= m_iMaxBullet[m_iBullet])
        {
            m_pGameInstance->Get_Player()->Set_State(CPlayer::STATE_HENDGUN_RELOAD);
        }
        else
        {
            m_iCurMotion = Shoot;
            m_fDamage = 16;
            m_pGameInstance->Player_To_Monster_Ray_Collison_Check();
        }

        m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon, 0.2f, 0.2f, 0.2f);
        m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerHP, 0.2f, -0.2f, 0.2f);
        m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon_Aim, 0.2f, 0.2f, -0.2f);
        bMotionChange = true;
        bLoop = false;
    }

    if (bMotionChange)
        m_pModelCom[m_pWeapon]->Set_Animation(m_iCurMotion, bLoop);

    if (true == m_pModelCom[m_pWeapon]->Play_Animation(fTimeDelta))
    {

        static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_State(CPlayer::STATE_IDLE);
        m_iCurMotion = Idle;
        m_pModelCom[m_pWeapon]->Set_Animation(m_iCurMotion, true);
    }
  
}

void CWeapon::Type2_Update(_float fTimeDelta)
{
    _bool bMotionChange = {false}, bLoop = {false};

    if (AssaultRifle == m_pWeapon)
    {
        if (*m_pParentState == CPlayer::STATE_ASSAULTRIFlLE_RELOAD && m_iCurMotion != Reload)
        {
            m_iBullet = CWeapon::AssaultRifle;
            m_iMaxBullet[m_iBullet] = 30;
            m_iCurMotion = Reload;
            bMotionChange = true;
            bLoop = false;
        }

        if (*m_pParentState == CPlayer::STATE_ASSAULTRIFlLE_SHOOT && m_iCurMotion != Shoot)
        {
            m_iMaxBullet[m_iBullet] -= 3;
            if (0 >= m_iMaxBullet[m_iBullet])
                static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_State(CPlayer::STATE_ASSAULTRIFlLE_RELOAD);
            else {
                m_iCurMotion = Shoot;
                m_fDamage = 18.f;
                m_pGameInstance->Player_To_Monster_Ray_Collison_Check();
            }
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon, 0.2f, 0.5f, 0.5f);
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerHP, 0.2f, -0.5f, 0.5f);
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon_Aim, 0.2f, 0.5f, -0.5f);
            bMotionChange = true;
            bLoop = false;
        }
    }
    else if (MissileGatling == m_pWeapon)
    {
        if (*m_pParentState == CPlayer::STATE_MissileGatling_RELOAD && m_iCurMotion != Reload)
        {
            m_iBullet = CWeapon::MissileGatling;
            m_iMaxBullet[m_iBullet] = 20;
            m_iCurMotion = Reload;
            bMotionChange = true;
            bLoop = false;
        }

        if (*m_pParentState == CPlayer::STATE_MissileGatling_SHOOT && m_iCurMotion != Shoot)
        {
            m_iMaxBullet[m_iBullet] -= 4;
            if (0 >= m_iMaxBullet[m_iBullet])
                static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_State(CPlayer::STATE_MissileGatling_RELOAD);
            else {
                m_iCurMotion = Shoot;
                m_fDamage = 20.f;
                m_pGameInstance->Player_To_Monster_Ray_Collison_Check();
            }
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon, 0.2f, 0.7f, 0.7f);
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerHP, 0.2f, -0.7f, 0.7f);
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon_Aim, 0.2f, 0.7f, -0.7f);
            bMotionChange = true;
            bLoop = false;
        }
    }
    else if (HeavyCrossbow == m_pWeapon)
    {
        if (*m_pParentState == CPlayer::STATE_HEAVYCROSSBOW_RELOAD && m_iCurMotion != Reload)
        {
            m_iBullet = CWeapon::HeavyCrossbow;
            m_iMaxBullet[m_iBullet] = 7;
            m_iCurMotion = Reload;
            bMotionChange = true;
            bLoop = false;
        }

        if (*m_pParentState == CPlayer::STATE_HEAVYCROSSBOW_SHOOT && m_iCurMotion != Shoot)
        {
            m_iMaxBullet[m_iBullet] -= 1;

            if (0 >= m_iMaxBullet[m_iBullet])
                static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_State(CPlayer::STATE_HEAVYCROSSBOW_RELOAD);
            else {
                m_iCurMotion = Shoot;
                m_fDamage = 50.f;
                m_pGameInstance->Player_To_Monster_Ray_Collison_Check();
            }
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon, 0.2f, 1.f, 1.f);
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerHP, 0.2f, -1.f, 1.f);
            m_pGameInstance->Set_UI_shaking(CUI::UIID_PlayerWeaPon_Aim, 0.2f, 1.f, -1.f);
           
            bMotionChange = true;
            bLoop = false;
        }
    }


    if (bMotionChange)
        m_pModelCom[m_pWeapon]->Set_Animation(m_iCurMotion, bLoop);

    if (true == m_pModelCom[m_pWeapon]->Play_Animation(fTimeDelta))
    {
        static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_State(CPlayer::STATE_IDLE2);
        m_iCurMotion = Idle;
        m_pModelCom[m_pWeapon]->Set_Animation(m_iCurMotion, true);
    }
 
}



void CWeapon::Choose_Weapon(const _uint& WeaponNum)
{
    m_pWeapon = WeaponNum;

    m_iBullet = static_cast<WeaPoneType>(WeaponNum);
    m_pModelCom[m_pWeapon]->Set_Animation(Idle, false);
}

HRESULT CWeapon::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_HendGun"), TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom[HendGun]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_AssaultRifle"), TEXT("Com_Model2"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom[AssaultRifle]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_MissileGatling"),
                                      TEXT("Com_Model3"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom[MissileGatling]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_HeavyCrossbow"), TEXT("Com_Model4"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom[HeavyCrossbow]))))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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

CWeapon* CWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWeapon* pInstance = new CWeapon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CWeapon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeapon::Clone(void* pArg)
{
    CWeapon* pInstance = new CWeapon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CWeapon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon::Free()
{
    __super::Free();

    for (size_t i = 0; i < WeaPoneType_END; i++) Safe_Release(m_pModelCom[i]);

}
