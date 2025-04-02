#include "stdafx.h"
#include "BillyBoom.h"
#include "GameInstance.h"
#include "Body_BillyBoom.h"
#include "BossBullet_Berrle.h"
#include "BossBullet_Laser.h"
#include "Bullet.h"


CBody_BillyBoom::CBody_BillyBoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartObject{pDevice, pContext}
{
}

CBody_BillyBoom::CBody_BillyBoom(const CBody_BillyBoom& Prototype) : CPartObject{Prototype}
{
}

HRESULT CBody_BillyBoom::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_BillyBoom::Initialize(void* pArg)
{

    CBody_BillyBoom_Desc* pDesc = static_cast<CBody_BillyBoom_Desc*>(pArg);

    m_pParentState   = pDesc->pParentState;
    m_RimDesc.eState = pDesc->pRimState;
   
  //  m_pParent        = pDesc->pObj;
 //   Safe_Release(m_pParent);
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    m_DATA_TYPE = CGameObject::DATA_MONSTER;
    if (FAILED(Add_Components()))
        return E_FAIL;
   m_fPlayAniTime = 0.5f;
   m_iEmissiveMashNum = 1;
 // m_pFindBonMatrix = Get_SocketMatrix("R_Canon_02");
   m_pFindAttBonMatrix[0] = Get_SocketMatrix("L_TopArm_04"); // 왼쪽 위 팔
   m_pFindAttBonMatrix[1] = Get_SocketMatrix("R_TopArm_04"); // 오른 쪽 위 팔
   m_pFindAttBonMatrix[2] = Get_SocketMatrix("R_TopArm_04"); // 위팔 모아 쏴
   m_pFindAttBonMatrix[3] = Get_SocketMatrix("R_Arm_04"); // 태양날리기

 
    return S_OK;
}

_int CBody_BillyBoom::Priority_Update(_float fTimeDelta)
{
    if (m_bDead) {
        return OBJ_DEAD;
    }
    return OBJ_NOEVENT;
}

void CBody_BillyBoom::Update(_float fTimeDelta)
{
 
    _matrix LaserMatrix =  XMMatrixRotationZ(XMConvertToRadians(m_BeamZ))* XMMatrixRotationY(XMConvertToRadians(-m_BeamY));

    if (m_bTurnBeam = true)
    {
      

            m_BeamZ += fTimeDelta*-8.f ;
            m_BeamY += fTimeDelta * 55.f;
            
        
    }



    _bool bMotionChange = {false}, bLoop = {false};
    if (*m_pParentState == CBillyBoom::ST_Idle && m_iCurMotion != CBillyBoom::ST_Idle)
    {
        m_bTurnBeam = false;
        m_iCurMotion = CBillyBoom::ST_Idle;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = false;
    }
    if (*m_pParentState == CBillyBoom::ST_Intro && m_iCurMotion != CBillyBoom::ST_Intro)
    {
        m_bTurnBeam = false;
        m_iCurMotion = CBillyBoom::ST_Intro;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = false;
    }
       
    if (m_iCurMotion == CBillyBoom::ST_Intro)
    {
        m_iEmissiveMashNum = 1;
        m_fEmissivePower = 5;
        m_fEmissiveColor = { 1.f, 0.5f, 0.0 }; // 노랑
        m_fTimeSum += fTimeDelta;

        if (m_fTimeSum > 1.f)
        {
            m_bEmissiveStart = true;
            m_fTimeSum = 0.f;
        }
    }
    else {
        m_bEmissiveStart = false;
    }

    if (*m_pParentState == CBillyBoom::ST_Comp_Poke_Front && m_iCurMotion != CBillyBoom::ST_Comp_Poke_Front)
    {
        m_bTurnBeam = false;
        m_iCurMotion = CBillyBoom::ST_Comp_Poke_Front;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = false;
    }
    if (*m_pParentState == CBillyBoom::ST_Comp_Idle && m_iCurMotion != CBillyBoom::ST_Comp_Idle)
    {
     
        m_bTurnBeam = false;
        m_iCurMotion = CBillyBoom::ST_Comp_Idle;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = true;
    }

    if (*m_pParentState == CBillyBoom::ST_Run_Front && m_iCurMotion != CBillyBoom::ST_Run_Front)
    {
        m_pGameInstance->PlayBGM(CSound::SOUND_MONSTER_SETB, L"ST_BillyBoom_FootStep.ogg", 0.5f);

        m_bTurnBeam = false;
        m_iCurMotion = CBillyBoom::ST_Run_Front;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = true;
    }

    if (*m_pParentState != CBillyBoom::ST_Run_Front) {
        m_pGameInstance->StopSound(CSound::SOUND_MONSTER_SETB);
    }

    if (m_iCurMotion == CBillyBoom::ST_Barre_Shoot) {
        m_fPlayAniTime = 1.f;
        m_bTurnBeam = false;
      
    }

    if (true == m_bBerrle) {
        m_fTimeSum += fTimeDelta;

        if (m_fTimeSum > 0.8f)
        {
            Make_Barre();
            m_fTimeSum = 0.f;
            m_bBerrle = false;
        }
    }

    if (*m_pParentState == CBillyBoom::ST_Barre_In && m_iCurMotion != CBillyBoom::ST_Barre_In)
    {
        m_pGameInstance->Play_Sound(L"ST_BillyBoom_Cast_Hand_Blast.ogg", CSound::SOUND_EFFECT, 0.5f);
        m_bTurnBeam = false;
        m_iCurMotion = CBillyBoom::ST_Barre_In;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = false;
    }
    if (m_iCurMotion == CBillyBoom::ST_Barre_PreShoot)
    {
        m_bTurnBeam = false;
        m_bBerrle = true;
        m_fPlayAniTime = 1.f;
    }

    if (m_iCurMotion == CBillyBoom::ST_Laser_ShootLoop)
    {  
        m_fPlayAniTime = 0.25f;

    }

    if ( m_iCurMotion == CBillyBoom::ST_Laser_PreShoot)
    {
 
        m_fPlayAniTime = 1.f;

    }

    if (*m_pParentState == CBillyBoom::ST_Laser_In && m_iCurMotion != CBillyBoom::ST_Laser_In)
    {
        m_BeamZ = 35.f;
        m_BeamY = -120.f;

        m_iCurMotion = CBillyBoom::ST_Laser_In;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = false;
    }

    if (*m_pParentState == CBillyBoom::ST_Comp_Idle)
    {
        m_DeadTimeSum += fTimeDelta;

        if(m_DeadTimeSum >2.f)
            m_bEmissiveStart = true;
    }


    if (*m_pParentState == CBillyBoom::ST_ShockWave_In && m_iCurMotion != CBillyBoom::ST_ShockWave_In)
    {

        
         m_pGameInstance->Play_Sound(L"ST_BillyBoom_From_Two_Blaster_To_One.ogg", CSound::SOUND_EFFECT, 0.5f);
        m_bTurnBeam = false;
        m_iCurMotion = CBillyBoom::ST_ShockWave_In;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = false;
    }


    if ( m_iCurMotion == CBillyBoom::ST_ShockWave_PreShoot)
    {
        m_bTurnBeam = false;
        m_fPlayAniTime = 1.f;

    }

    if ( m_iCurMotion == CBillyBoom::ST_ShockWave_Shoot)
    {
        m_bTurnBeam = false;
     
        m_fPlayAniTime = 1.f;

    }

    if ( m_iCurMotion == CBillyBoom::ST_ShockWave_Out)
    {
        m_bTurnBeam = false;

        m_fPlayAniTime = 1.f;

    }

    if (*m_pParentState == CBillyBoom::ST_Bash_PreShoot && m_iCurMotion != CBillyBoom::ST_Bash_PreShoot)
    {
        m_bTurnBeam = false;
        m_iCurMotion = CBillyBoom::ST_Bash_PreShoot;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = false;
    }
    if ( m_iCurMotion == CBillyBoom::ST_Bash_Shoot)
    {
        m_bTurnBeam = false;
  
        m_fPlayAniTime = 1.f;

    }


    if (*m_pParentState == CBillyBoom::ST_Stun_Start && m_iCurMotion != CBillyBoom::ST_Stun_Start)
    {
        m_bTurnBeam = false;
        m_iCurMotion = CBillyBoom::ST_Stun_Start;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = false;
    }


    if (*m_pParentState == CBillyBoom::ST_Stun_Pose && m_iCurMotion != CBillyBoom::ST_Stun_Pose)
    {
        m_bTurnBeam = false;
        m_iCurMotion = CBillyBoom::ST_Stun_Pose;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = false;
    }
    
    if (*m_pParentState == CBillyBoom::ST_Stun_Recover && m_iCurMotion != CBillyBoom::ST_Stun_Recover)
    {
        m_bTurnBeam = false;
        m_iCurMotion = CBillyBoom::ST_Stun_Recover;
        m_fPlayAniTime = 1.f;
        bMotionChange = true;
        bLoop = false;
    }

    if (*m_RimDesc.eState == RIM_LIGHT_DESC::STATE_RIM)
    {
        m_RimDesc.fcolor = { 1.f,1.f,1.f,1.f };
        m_RimDesc.iPower = 1;
    }


    if (*m_RimDesc.eState == RIM_LIGHT_DESC::STATE_NORIM) {
        m_RimDesc.fcolor = { 0.f,0.f,0.f,0.f };
        m_RimDesc.iPower = 1;
    }
    
    if (bMotionChange)
        m_pModelCom->Set_Animation(m_iCurMotion, bLoop);


  
    if (true == m_pModelCom->Play_Animation(fTimeDelta * m_fPlayAniTime))
    {

        if (m_iCurMotion == CBillyBoom::ST_Bash_Shoot)
            m_bFinishAni = true;
        else
        if (m_iCurMotion == CBillyBoom::ST_ShockWave_Shoot)
                m_bFinishAni = true;
        else
        if (m_iCurMotion == CBillyBoom::ST_Laser_ShootLoop)
                        m_bFinishAni = true;
        else
            if (m_iCurMotion == CBillyBoom::ST_Barre_Shoot)
                m_bFinishAni = true;
        else
        if (m_iCurMotion == CBillyBoom::ST_Bash_PreShoot)
        {
          m_iParentSt = CBillyBoom:: ST_Bash_Shoot;
            m_iCurMotion = CBillyBoom::ST_Bash_Shoot;
            m_pModelCom->Set_Animation(m_iCurMotion, false);
        }
        else
        if (m_iCurMotion == CBillyBoom::ST_ShockWave_In)
        {
            m_iParentSt = CBillyBoom::ST_ShockWave_PreShoot;
            m_iCurMotion = CBillyBoom::ST_ShockWave_PreShoot;
            m_pModelCom->Set_Animation(m_iCurMotion, false);
        }
        else
        if (m_iCurMotion == CBillyBoom::ST_ShockWave_PreShoot)
        {
            Make_ShockWave();
            m_iParentSt = CBillyBoom::ST_ShockWave_Shoot;
            m_iCurMotion = CBillyBoom::ST_ShockWave_Shoot;
            m_pModelCom->Set_Animation(m_iCurMotion, false);
        }
        else
        if (m_iCurMotion == CBillyBoom::ST_Laser_In)
        {
            m_bTurnBeam = false;
            m_iParentSt = CBillyBoom::ST_Laser_PreShoot;
            m_iCurMotion = CBillyBoom::ST_Laser_PreShoot;
            m_pModelCom->Set_Animation(m_iCurMotion, false);
        }
        else
        if (m_iCurMotion == CBillyBoom::ST_Laser_PreShoot)
        {
      
            Make_Laser();
            m_iParentSt = CBillyBoom::ST_Laser_ShootLoop;
            m_iCurMotion = CBillyBoom::ST_Laser_ShootLoop;
            m_pModelCom->Set_Animation(m_iCurMotion, false);
        }
        else
        if(m_iCurMotion == CBillyBoom::ST_Barre_In)
        {
            m_pDamage = 20.f;
            m_iParentSt = CBillyBoom::ST_Barre_PreShoot;
           m_iCurMotion = CBillyBoom::ST_Barre_PreShoot;
           m_pModelCom->Set_Animation(m_iCurMotion, false);
        }
        else
        if (m_iCurMotion == CBillyBoom::ST_Barre_PreShoot)
        {
            m_iParentSt = CBillyBoom::ST_Barre_Shoot;
            m_iCurMotion = CBillyBoom::ST_Barre_Shoot;
            m_pModelCom->Set_Animation(m_iCurMotion, false);
        }
        else
        if (m_iCurMotion != CBillyBoom::ST_Intro) {
            m_bFinishAni = true;
            m_iCurMotion = CBillyBoom::ST_Idle;
            m_pModelCom->Set_Animation(m_iCurMotion, false);
        }
    }
    else
    {
        m_bFinishAni = false;
       if(m_iCurMotion == CBillyBoom::ST_Comp_Idle)
        m_pModelCom->Set_Animation(m_iCurMotion, false);
    }
    if ( m_iCurMotion == CBillyBoom::ST_Laser_ShootLoop) {
        Set_BoneUpdateMatrix("R_TopArm_04", LaserMatrix);
        Set_BoneUpdateMatrix("L_TopArm_04", LaserMatrix);
    }






    m_pHeanColl->Update(XMLoadFloat4x4(&m_HeandWorldMatrix));
}

void CBody_BillyBoom::Late_Update(_float fTimeDelta)
{
        if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;

   

        _matrix SocketMatrix = XMLoadFloat4x4(m_pFindAttBonMatrix[3]);

        for (size_t i = 0; i < 3; i++)
            SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]); // 항등으로 만들어서 넣겠다..

        XMStoreFloat4x4(&m_HeandWorldMatrix,
            SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));


        if (FAILED(m_pGameInstance->Add_Collider(m_pDamage, m_pHeanColl)))
            return;

        m_pGameInstance->Add_DebugComponents(m_pHeanColl);
    __super::Late_Update(fTimeDelta);
}

HRESULT CBody_BillyBoom::Render()
{

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;


    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {

        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0,
                                                             "g_DiffuseTexture")))
            return E_FAIL;

     
        if (true == m_bEmissiveStart) {

            if (m_iCurMotion != CBillyBoom::ST_Comp_Idle) {
                if (i == m_iEmissiveMashNum)
                    m_bEmissive = true;
                else
                    m_bEmissive = false;
            }
            else  m_bEmissive = true;

            if(4 == m_iEmissiveMashNum)
                m_bEmissive = true;


            if (FAILED(m_pShaderCom->Bind_Bool("g_bEmissive", m_bEmissive)))
                return E_FAIL;
        }

        if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(2)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}



HRESULT CBody_BillyBoom::Make_Barre()
{
    _vector BHend_Local_Pos = { m_pFindAttBonMatrix[3]->_41, m_pFindAttBonMatrix[3]->_42,  m_pFindAttBonMatrix[3]->_43,  m_pFindAttBonMatrix[3]->_44 };
   _vector vHPos = XMVector3TransformCoord(BHend_Local_Pos, XMLoadFloat4x4(&m_WorldMatrix));

   m_pGameInstance->Play_Sound(L"ST_BillyBoom_Hand_Blast_Shoot.ogg", CSound::SOUND_EFFECT,0.5f);

   CBossBullet_Berrle::CBossBullet_Berrle_DESC Desc{};
   Desc.fSpeedPerSec = 20.f;
   Desc.pTagetPos = m_AttackDir;
   Desc.vPos = vHPos;
   Desc.state = &m_iCurMotion;
   Desc.iSkillType = CSkill::SKill::STYPE_BERRLE;
   m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), CGameObject::SKILL, L"Prototype GameObject_BossBullet_Berrle", nullptr, 0, &Desc, 0);


   CBullet::CBULLET_DESC BDesc{};
   BDesc.fSpeedPerSec = 20.f;
   BDesc.pTagetPos = m_AttackDir;
   BDesc.vPos = vHPos;
   BDesc.state = &m_iCurMotion;
   BDesc.fDamage = &m_pDamage;
   BDesc.iActorType = CSkill::MONSTER::TYPE_BILLYBOOM;

   m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), CGameObject::SKILL, L"Prototype GameObject_Bullet", nullptr, 0, &BDesc, 0);

   return S_OK;
}

HRESULT CBody_BillyBoom::Make_ShockWave()
{
    m_pGameInstance->Play_Sound(L"ST_BillyBoom_Shockwave_Shoot.ogg", CSound::SOUND_EFFECT, 0.5f);
    _vector SHend_Local_Pos = { m_pFindAttBonMatrix[2]->_41 , m_pFindAttBonMatrix[2]->_42 + 1.f,  m_pFindAttBonMatrix[2]->_43,  m_pFindAttBonMatrix[2]->_44 };
    _vector vHPos = XMVector3TransformCoord(SHend_Local_Pos, XMLoadFloat4x4(&m_WorldMatrix));

    m_pDamage = 10.f;
    CBullet::CBULLET_DESC Desc{};
    Desc.fSpeedPerSec = 20.f;
    Desc.pTagetPos = m_AttackDir;
    Desc.vPos = vHPos;
    Desc.state = &m_iCurMotion;
    Desc.fDamage = &m_pDamage;
    Desc.iSkillType = CSkill::SKill:: STYPE_SHOCKWAVE;

    m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), CGameObject::SKILL, L"Prototype GameObject_Bullet", nullptr, 0, &Desc, 0);

    return S_OK;
}

HRESULT CBody_BillyBoom::Make_Laser()
{
    m_pDamage = 10.f; 
    m_pGameInstance->Play_Sound(L"ST_Enemy_Laser_Loop_Start.ogg", CSound::SOUND_EFFECT, 0.5f);
    m_pGameInstance->PlayBGM(CSound::SOUND_EFFECT_LASER, L"ST_Enemy_Laser_Loop.ogg", 0.5f);

    _vector RHend_Local_Pos = { m_pFindAttBonMatrix[1]->_41, m_pFindAttBonMatrix[1]->_42+0.5f,  m_pFindAttBonMatrix[1]->_43 +3.f,  m_pFindAttBonMatrix[1]->_44 };
    _vector LHend_Local_Pos = { m_pFindAttBonMatrix[0]->_41, m_pFindAttBonMatrix[0]->_42+0.5f,  m_pFindAttBonMatrix[0]->_43+3.f,  m_pFindAttBonMatrix[0]->_44 };

    _vector RHPos = XMVector3TransformCoord(RHend_Local_Pos, XMLoadFloat4x4(&m_WorldMatrix));
    _vector LHPos = XMVector3TransformCoord(LHend_Local_Pos, XMLoadFloat4x4(&m_WorldMatrix));


   CBossBullet_Berrle::CBossBullet_Berrle_DESC Desc{};
   Desc.fSpeedPerSec = 20.f;;
   Desc.fDamage = &m_pDamage;
   Desc.vPos = RHPos;
   Desc.state = &m_iCurMotion;
   Desc.LaserRightLeft = true;
   Desc.LaserpParentMatrix = &m_WorldMatrix;
   Desc.LaserpSocketMatrix = m_pFindAttBonMatrix[1];
   Desc.iSkillType = CSkill::STYPE_LASER;
   m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), CGameObject::SKILL, L"Prototype GameObject_BossBullet_Berrle", nullptr, 0, &Desc, 0);
  

   ZeroMemory(&Desc, sizeof(CBossBullet_Berrle::CBossBullet_Berrle_DESC));
    Desc.fSpeedPerSec = 20.f;
    Desc.fDamage = &m_pDamage;
    Desc.vPos = LHPos;
    Desc.state = &m_iCurMotion;
    Desc.LaserRightLeft = false;
    Desc.LaserpParentMatrix = &m_WorldMatrix;
    Desc.LaserpSocketMatrix = m_pFindAttBonMatrix[0];
    Desc.iSkillType = CSkill::STYPE_LASER;
    m_pGameInstance->Add_GameObject_To_Layer(m_pGameInstance->Get_iCurrentLevel(), CGameObject::SKILL, L"Prototype GameObject_BossBullet_Berrle", nullptr, 0, &Desc, 0);
   
    m_bTurnBeam = true;

    return S_OK;
}

void CBody_BillyBoom::SetDir()
{  
  m_AttackDir = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION)
        - m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);
}



HRESULT CBody_BillyBoom::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
                                      reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_ComPonent_BillyBoom"), TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;


     CBounding_OBB::BOUND_OBB_DESC OBBDesc{};

     OBBDesc.vExtents = _float3(1.f, 0.5f, 1.f);
     OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);
     OBBDesc.vRotation = { 0.f, 0.f, 0.f };
     if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider_OBB"),
         reinterpret_cast<CComponent**>(&m_pHeanColl), &OBBDesc)))
         return E_FAIL;
    
    return S_OK;
}

HRESULT CBody_BillyBoom::Bind_ShaderResources()
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
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Bool("g_TagetBool", *m_RimDesc.eState)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Int("g_RimPow", m_RimDesc.iPower)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &m_RimDesc.fcolor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Bool("g_TagetDeadBool", m_iCurMotion == CBillyBoom::ST_Stun_Start)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Float("g_EmissivePower", m_fEmissivePower)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_EmissiveColor", &m_fEmissiveColor, sizeof(_float3))))
        return E_FAIL;

    if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, 1, aiTextureType_EMISSIVE, 0,
        "g_EmissiveTexture")))
        return E_FAIL;

    return S_OK;
}

CBody_BillyBoom* CBody_BillyBoom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_BillyBoom* pInstance = new CBody_BillyBoom(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBody_BillyBoom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_BillyBoom::Clone(void* pArg)
{
    CBody_BillyBoom* pInstance = new CBody_BillyBoom(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBody_BillyBoom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_BillyBoom::Free()
{
    __super::Free();
    Safe_Release(m_pHeanColl);
}
