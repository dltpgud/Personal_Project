#include "stdafx.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Player.h"
#include "UI.h"
#include "ShootEffect.h"
#include "ShootingUI.h"
#include "PlayerBullet.h"
#include "Body_Player.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartObject{ pDevice, pContext }
{
}

CWeapon::CWeapon(const CWeapon& Prototype) : CPartObject{ Prototype }
{
}

HRESULT CWeapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
    WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);
    m_pParentMatrix = pDesc->pParentMatrix;
    m_pSocketMatrix = pDesc->pSocketMatrix;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;
  
    if (FAILED(Init_Weapon()))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

     if (FAILED(Init_CallBack()))
        return E_FAIL;

    m_pTransformCom->Set_Scaling(0.01f, 0.01f, 0.01f);

    m_iWeapon = CWeapon::HendGun;
    return S_OK;
}

void CWeapon::Priority_Update(_float fTimeDelta)
{
    m_pGameInstance->UI_shaking(UIID_PlayerHP, fTimeDelta);
    m_pGameInstance->UI_shaking(UIID_PlayerWeaPon, fTimeDelta);
    m_pGameInstance->UI_shaking(UIID_PlayerWeaPon_Aim, fTimeDelta);
    m_pGameInstance->UI_shaking(UIID_BossHP, fTimeDelta);
}

void CWeapon::Update(_float fTimeDelta)
{
   
}

void CWeapon::Late_Update(_float fTimeDelta)
{
   _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
  
    for (size_t i = 0; i < 3; i++)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]); 
  
    XMStoreFloat4x4(&m_WorldMatrix,
         m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
 

    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
}

HRESULT CWeapon::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_vecWeaPone[m_iWeapon].pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_vecWeaPone[m_iWeapon].pModelCom->Bind_Material_ShaderResource(
                m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
            "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_vecWeaPone[m_iWeapon].pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (m_iWeapon != MissileGatling)
        {
            if (FAILED(m_vecWeaPone[m_iWeapon].pModelCom->Bind_Material_ShaderResource(
                    m_pShaderCom, i, aiTextureType_EMISSIVE, 0,
                "g_EmissiveTexture")))
                return E_FAIL;

             if (FAILED(m_pShaderCom->Begin(3)))
                return E_FAIL;
        }
        else 
            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;

       m_vecWeaPone[m_iWeapon].pModelCom->Render(i);
    }

    return S_OK;
}

void CWeapon::Choose_Weapon(const _uint& WeaponNum)
{
    m_iWeapon = WeaponNum;
    m_vecWeaPone[m_iWeapon].pModelCom->Set_Animation(WS_IDLE, false);
}


void CWeapon::Weapon_CallBack(_int WeaPonType, _uint AnimIdx, _int Duration, function<void()> func)
{
    m_vecWeaPone[WeaPonType].pModelCom->Callback(AnimIdx, Duration, func);
}

HRESULT CWeapon::Make_Bullet()
{
    if (m_iWeapon >= WeaPoneType_END || m_iWeapon < HendGun)
        return E_FAIL;
 
    _int iCount{1};
    switch (m_iWeapon)
    {
    case CWeapon::HendGun: 
          m_vecWeaPone[m_iWeapon].fEmissvePower = 2.f;
          break;
    case CWeapon::AssaultRifle: 
          m_vecWeaPone[m_iWeapon].fEmissvePower = 2.f;
          break;
    case CWeapon::MissileGatling: 
          iCount = 4; 
          m_vecWeaPone[m_iWeapon].fEmissvePower = 2.f;
          break;
    case CWeapon::HeavyCrossbow: 
          m_vecWeaPone[m_iWeapon].fEmissvePower = 3.f;
          break;
    default: break;
    }

    m_vecWeaPone[m_iWeapon].iCurBullet -= iCount;

    m_pGameInstance->Player_To_Monster_Ray_Collison_Check();
   
    _float3	vPickPos{};
    _vector At{};

     At = XMLoadFloat4(m_pGameInstance->Get_CamPosition()) + XMLoadFloat4(m_pGameInstance->Get_CamLook()) * 10.f;

    _vector Hend_Local_Pos = {m_vecWeaPone[m_iWeapon].pBoneMatrix->_41 * m_vecWeaPone[m_iWeapon].iBulletOffset.x,
                              m_vecWeaPone[m_iWeapon].pBoneMatrix->_42 * m_vecWeaPone[m_iWeapon].iBulletOffset.y,
                              m_vecWeaPone[m_iWeapon].pBoneMatrix->_43 * m_vecWeaPone[m_iWeapon].iBulletOffset.z,
                              m_vecWeaPone[m_iWeapon].pBoneMatrix->_44};

    _vector vHPos = XMVector3TransformCoord(Hend_Local_Pos, XMLoadFloat4x4(&m_WorldMatrix));

    _vector Local_Pos = {m_vecWeaPone[m_iWeapon].pBoneMatrix->_41 * m_vecWeaPone[m_iWeapon].iBulletOffset.x,
                         m_vecWeaPone[m_iWeapon].pBoneMatrix->_42 * m_vecWeaPone[m_iWeapon].iBulletOffset.y + 5.f,
                         m_vecWeaPone[m_iWeapon].pBoneMatrix->_43 * m_vecWeaPone[m_iWeapon].iBulletOffset.z,
                         m_vecWeaPone[m_iWeapon].pBoneMatrix->_44};

   CShootEffect::CShootEffect_DESC pDesc{};
    pDesc.vPos = vHPos;
    pDesc.vTgetPos = { m_WorldMatrix._41, m_WorldMatrix._42,  m_WorldMatrix._43,  m_WorldMatrix._44 };
    pDesc.Local = Local_Pos;
    pDesc.WorldPtr = &m_WorldMatrix;
    pDesc.iWeaponType = m_iWeapon;
    m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"),
                                             L"Prototype GameObject_ShootEffect", &pDesc);
 
    CPlayerBullet::CPlayerBullet_DESC Desc{};
    Desc.fSpeedPerSec = 150.f;
    Desc.pTagetPos = At;
    Desc.vPos = vHPos;
    Desc.iSkillType = m_iWeapon;
    Desc.fDamage = &m_fDamage;
    Desc.Local = Hend_Local_Pos;
    Desc.WorldPtr = &m_WorldMatrix;
    Desc.iWeaponType = m_iWeapon;
    m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), TEXT("Layer_Skill"),
                                             L"Prototype GameObject_PlayerBullet", &Desc);
    return S_OK;
}

HRESULT CWeapon::Init_Weapon()
{
    m_vecWeaPone.resize(WeaPoneType::WeaPoneType_END);

    WEAPON_NODE_DESC pWDesc{};
    pWDesc.iBodyType = BODY_TYPE::T00;
    pWDesc.iMaxBullet = 15;
    pWDesc.iCurBullet = pWDesc.iMaxBullet;
    pWDesc.fEmissvePower = 1.f;
    pWDesc.iBulletOffset = {1.7f, 2.0f, 3.0f};
    pWDesc.fPreEmissvePower = pWDesc.fEmissvePower;
    m_vecWeaPone[CWeapon::HendGun] = pWDesc;

    pWDesc.iBodyType = BODY_TYPE::T01;
    pWDesc.iMaxBullet = 30;
    pWDesc.iCurBullet = pWDesc.iMaxBullet;
    pWDesc.fEmissvePower = 1.f;
    pWDesc.iBulletOffset = {1.1f, 0.8f, 1.2f};
    pWDesc.fPreEmissvePower = pWDesc.fEmissvePower;
    m_vecWeaPone[CWeapon::AssaultRifle] = pWDesc;

    pWDesc.iBodyType = BODY_TYPE::T01;
    pWDesc.iMaxBullet = 20;
    pWDesc.iCurBullet = pWDesc.iMaxBullet;
    pWDesc.fEmissvePower = 1.f;
    pWDesc.iBulletOffset = {1.f, 1.f, 1.5f};
    pWDesc.fPreEmissvePower = pWDesc.fEmissvePower;
    m_vecWeaPone[CWeapon::MissileGatling] = pWDesc;

    pWDesc.iBodyType = BODY_TYPE::T01;
    pWDesc.iMaxBullet = 7;
    pWDesc.iCurBullet = pWDesc.iMaxBullet;
    pWDesc.fEmissvePower = 1.f;
    pWDesc.iBulletOffset = {1.f, 1.f, 1.5f};
    pWDesc.fPreEmissvePower = pWDesc.fEmissvePower;
    m_vecWeaPone[CWeapon::HeavyCrossbow] = pWDesc;

    return S_OK;
}

HRESULT CWeapon::Init_CallBack()
{
    m_vecWeaPone[CWeapon::HendGun].pModelCom->Callback(CWeapon::WS_IDLE, 0, [this]() { ResetEmissive(); });
    m_vecWeaPone[CWeapon::AssaultRifle].pModelCom->Callback(CWeapon::WS_IDLE, 0, [this]() { ResetEmissive(); });
    m_vecWeaPone[CWeapon::MissileGatling].pModelCom->Callback(CWeapon::WS_IDLE, 0, [this]() { ResetEmissive(); });
    m_vecWeaPone[CWeapon::HeavyCrossbow].pModelCom->Callback(CWeapon::WS_IDLE, 0, [this]() { ResetEmissive(); });

    m_vecWeaPone[CWeapon::HendGun].pModelCom->Callback(CWeapon::WS_RELOAD, 30, [this]() { ResetBullet(); });
    m_vecWeaPone[CWeapon::AssaultRifle].pModelCom->Callback(CWeapon::WS_RELOAD, 30, [this]() { ResetBullet(); });
    m_vecWeaPone[CWeapon::MissileGatling].pModelCom->Callback(CWeapon::WS_RELOAD, 30, [this]() { ResetBullet(); });
    m_vecWeaPone[CWeapon::HeavyCrossbow].pModelCom->Callback(CWeapon::WS_RELOAD, 40, [this]() { ResetBullet(); });

    m_vecWeaPone[CWeapon::HendGun].pModelCom->Callback(CWeapon::WS_SHOOT, 3, [this]() { Make_Bullet(); });
    m_vecWeaPone[CWeapon::AssaultRifle].pModelCom->Callback(CWeapon::WS_SHOOT, 3, [this]() { Make_Bullet(); });
    m_vecWeaPone[CWeapon::MissileGatling].pModelCom->Callback(CWeapon::WS_SHOOT, 3, [this]() { Make_Bullet(); });
    m_vecWeaPone[CWeapon::HeavyCrossbow].pModelCom->Callback(CWeapon::WS_SHOOT, 3, [this]() { Make_Bullet(); });

    return S_OK;
}

HRESULT CWeapon::Set_Animation( _int Index, _bool IsLoop)
{
    m_vecWeaPone[m_iWeapon].pModelCom->Set_Animation(Index, IsLoop);
    return S_OK;
}

_bool CWeapon::Play_Animation(_float fTimeDelta)
{
    return m_vecWeaPone[m_iWeapon].pModelCom->Play_Animation(fTimeDelta);
}

_int CWeapon::Get_Weapon_Body_Type()
{
    return m_vecWeaPone[m_iWeapon].iBodyType;
}

HRESULT CWeapon::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_HendGun"), TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_vecWeaPone[CWeapon::HendGun].pModelCom))))
        return E_FAIL;

    m_vecWeaPone[CWeapon::HendGun].pBoneMatrix = m_vecWeaPone[CWeapon::HendGun].pModelCom->Get_BoneMatrix("W_Trigger");

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_AssaultRifle"), TEXT("Com_Model2"),
                                      reinterpret_cast<CComponent**>(&m_vecWeaPone[CWeapon::AssaultRifle].pModelCom))))
        return E_FAIL;

    m_vecWeaPone[CWeapon::AssaultRifle].pBoneMatrix = m_vecWeaPone[CWeapon::AssaultRifle].pModelCom->Get_BoneMatrix("W_Bayonet");

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_MissileGatling"),TEXT("Com_Model3"),
                                      reinterpret_cast<CComponent**>(&m_vecWeaPone[CWeapon::MissileGatling].pModelCom))))
        return E_FAIL;

    m_vecWeaPone[CWeapon::MissileGatling].pBoneMatrix = m_vecWeaPone[CWeapon::MissileGatling].pModelCom->Get_BoneMatrix("W_Rotator");

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_HeavyCrossbow"), TEXT("Com_Model4"),
                                      reinterpret_cast<CComponent**>(&m_vecWeaPone[CWeapon::HeavyCrossbow].pModelCom))))
        return E_FAIL;

    m_vecWeaPone[CWeapon::HeavyCrossbow].pBoneMatrix = m_vecWeaPone[CWeapon::HeavyCrossbow].pModelCom->Get_BoneMatrix("W_Front_Middle");

    return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;
  
    if (FAILED(m_pShaderCom->Bind_RawValue("g_EmissivePower", &m_vecWeaPone[m_iWeapon].fEmissvePower, sizeof(_float))))
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

    for (size_t i = 0; i < m_vecWeaPone.size(); ++i) { Safe_Release(m_vecWeaPone[i].pModelCom); }
    m_vecWeaPone.clear();
    m_vecWeaPone.shrink_to_fit();
}
