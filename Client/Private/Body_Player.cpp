#include "stdafx.h"
#include "Body_Player.h"
#include "GameInstance.h"
#include "Player.h"

CBody_Player::CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CPartObject{ pDevice, pContext }
{
}

CBody_Player::CBody_Player(const CBody_Player& Prototype) : CPartObject{ Prototype }
{
}

HRESULT CBody_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Player::Initialize(void* pArg)
{
	BODY_PLAYER_DESC* pDesc = static_cast<BODY_PLAYER_DESC*>(pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CBody_Player::Priority_Update(_float fTimeDelta)
{

}

void CBody_Player::Update(_float fTimeDelta)
{
	
}

void CBody_Player::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CBody_Player::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CBody_Player::Set_Animation(_int Body, _int Index, _bool IsLoop)
{
    m_pModelCom->Set_Animation(BodyMotion(Body, Index), IsLoop);
}

_bool CBody_Player::Play_Animation(_float fTimeDelta)
{
    return m_pModelCom->Play_Animation(fTimeDelta);
}

void CBody_Player::BodyCallBack(_int Body, _uint AnimIdx, _int Duration, function<void()> func)
{ 
    m_pModelCom->Callback(BodyMotion(Body,AnimIdx), Duration, func);
}

_int CBody_Player::BodyMotion(_int Body, _int Motion)
{
  _int iCurMotion{};

  switch (Motion)
  {
  case BODY_STATE::BODY_IDLE: 
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_IDLE1 : iCurMotion = STATE_IDLE2; 
       break;
  case BODY_STATE::BODY_SWICH: 
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_SWITCHIN : iCurMotion = STATE_SWITCHIN2; 
       break;
  case BODY_STATE::BODY_STUN: 
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_STUN : iCurMotion = STATE_STUN2; 
       break;
  case BODY_STATE::BODY_RUN: 
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_RUN : iCurMotion = STATE_RUN2; 
       break;
  case BODY_STATE::BODY_SPRINT:
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_SPRINT : iCurMotion = STATE_SPRINT2;
       break;
  case BODY_STATE::BODY_JUMP_RUN_LOOP:
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_JUMP_RUN_LOOP : iCurMotion = STATE_JUMP_RUN_LOOP2;
       break;
  case BODY_STATE::BODY_JUMP_FALL:
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_JUMP_FALL : iCurMotion = STATE_JUMP_FALL2;
       break;
  case BODY_STATE::BODY_COOLING_LONG:
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_COOLING_LONG : iCurMotion = STATE_COOLING_LONG2;
       break;
  case BODY_STATE::BODY_COOLING_SHORT:
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_COOLING_SHORT : iCurMotion = STATE_COOLING_SHORT2;
       break;
  case BODY_STATE::BODY_DEATH:
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_DEATH : iCurMotion = STATE_DEATH2;
       break;
  case BODY_STATE::BODY_GENERIC_SECONDARY_SHOOT:
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_GENERIC_SECONDARY_SHOOT : iCurMotion = STATE_GENERIC_SECONDARY_SHOOT2;
       break;
  case BODY_STATE::BODY_GENERIC_STAP:
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_GENERIC_STAP : iCurMotion = STATE_GENERIC_STAP2;
       break;
  case BODY_STATE::BODY_SPTINT_FALL:
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_SPTINT_FALL : iCurMotion = STATE_SPTINT_FALL2;
       break;
  case BODY_STATE::BODY_SPTINT_LOOP:
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_SPTINT_LOOP : iCurMotion = STATE_SPTINT_LOOP2;
       break;
  case BODY_STATE::BODY_WEAPON_THROW1:
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_WEAPON_THROW1 : iCurMotion = STATE_WEAPON_THROW2;
       break;
  case BODY_STATE::BODY_WEAPON_THROW2:
       Body == BODY_TYPE::T00 ? iCurMotion = STATE_WEAPON_THROW3 : iCurMotion = STATE_WEAPON_THROW4;
       break;	     
  default:
       iCurMotion = Motion;
       break;
  }

  return iCurMotion;
}

//void CBody_Player::Type0_Update(_float fTimeDelta)
//{
//	_bool bMotionChange = { false }, bLoop = { false };
//
//	if (*m_pParentState == CPlayer::STATE_SPRINT && m_iCurMotion != CPlayer::STATE_SPRINT)
//	{
//		m_pGameInstance->PlayBGM(CSound::CHANNELID::SOUND_EFFECT_BGM2, L"ST_Player_Footstep_Scuff_Sand_05.ogg", 0.7f);
//		m_iCurMotion = CPlayer::STATE_SPRINT;
//		bMotionChange = true;
//		bLoop = true;
//	}
//	if (*m_pParentState == CPlayer::STATE_JUMP_RUN_LOOP && m_iCurMotion != CPlayer::STATE_JUMP_RUN_LOOP)
//	{
//		m_iCurMotion = CPlayer::STATE_JUMP_RUN_LOOP;
//		bMotionChange = true;
//		bLoop = true;
//	}
//	if (*m_pParentState == CPlayer::STATE_JUMP_RUN_HIGH && m_iCurMotion != CPlayer::STATE_JUMP_RUN_HIGH)
//	{
//		m_iCurMotion = CPlayer::STATE_JUMP_RUN_HIGH;
//		bMotionChange = true;
//		bLoop = true;
//	}
//	if (*m_pParentState == CPlayer::STATE_RUN && m_iCurMotion != CPlayer::STATE_RUN)
//	{
//		m_pGameInstance->PlayBGM(CSound::CHANNELID::SOUND_EFFECT_BGM,L"ST_Player_Footstep_Scuff_Sand.ogg", 0.7f);
//		m_iCurMotion = CPlayer::STATE_RUN;
//		bMotionChange = true;
//		bLoop = true;
//	}
//	
//	if (*m_pParentState != CPlayer::STATE_SPRINT)
//	{
//		m_pGameInstance->StopSound(CSound::CHANNELID::SOUND_EFFECT_BGM2);
//	}
//
//	if(*m_pParentState != CPlayer::STATE_RUN)
//	{
//		m_pGameInstance->StopSound(CSound::CHANNELID::SOUND_EFFECT_BGM);
//	}
//
//	if (*m_pParentState == CPlayer::STATE_SWITCHIN && m_iCurMotion != CPlayer::STATE_SWITCHIN)
//	{
//		m_iCurMotion = CPlayer::STATE_SWITCHIN;
//		bMotionChange = true;
//		bLoop = false;
//	}
//
//	if (*m_pParentState == CPlayer::STATE_STUN && m_iCurMotion != CPlayer::STATE_STUN)
//	{
//		m_iCurMotion = CPlayer::STATE_STUN;
//		bMotionChange = true;
//		bLoop = false;
//	}
//
//	if (*m_pParentState == CPlayer::STATE_IDLE && m_iCurMotion != CPlayer::STATE_IDLE)
//	{
//		m_iCurMotion = CPlayer::STATE_IDLE;
//		bMotionChange = true;
//		bLoop = true;
//	}
//
//	if (*m_pParentState == CPlayer::STATE_HENDGUN_RELOAD && m_iCurMotion != CPlayer::STATE_HENDGUN_RELOAD)
//	{
//		m_iCurMotion = CPlayer::STATE_HENDGUN_RELOAD;
//		bMotionChange = true;
//		bLoop = false;
//	}
//	if (*m_pParentState == CPlayer::STATE_HENDGUN_SHOOT && m_iCurMotion != CPlayer::STATE_HENDGUN_SHOOT)
//	{
//			m_iCurMotion = CPlayer::STATE_HENDGUN_SHOOT;
//			bMotionChange = true;
//			bLoop = false;
//	
//	}
//
//	if (bMotionChange)
//		m_pModelCom->Set_Animation(m_iCurMotion, bLoop);
//
//	if (true == m_pModelCom->Play_Animation(fTimeDelta))
//	{
//		m_bFinishAni = true;
//		static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_HitLock(false);
//		static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_State(CPlayer::STATE_IDLE);
//		m_iCurMotion = CPlayer::STATE_IDLE;
//		m_pModelCom->Set_Animation(m_iCurMotion, true);
//	}else
//		m_bFinishAni = false;
//
//}

//void CBody_Player::Type2_Update(_float fTimeDelta)
//{
//	_bool bMotionChange = { false }, bLoop = { false };
//
//	if (*m_pParentState == CPlayer::STATE_JUMP_RUN_HIGH2 && m_iCurMotion != CPlayer::STATE_JUMP_RUN_HIGH2)
//	{
//		m_fPlayAniTime = 1.f;
//		m_iCurMotion = CPlayer::STATE_JUMP_RUN_HIGH2;
//		bMotionChange = true;
//		bLoop = true;
//	}
//
//	if (*m_pParentState == CPlayer::STATE_SPRINT2 && m_iCurMotion != CPlayer::STATE_SPRINT2)
//	{
//		m_pGameInstance->PlayBGM(CSound::CHANNELID::SOUND_EFFECT_BGM2, L"ST_Player_Footstep_Scuff_Sand_05.ogg", 0.7f);
//		m_fPlayAniTime = 1.f;
//		m_iCurMotion = CPlayer::STATE_SPRINT2;
//		bMotionChange = true;
//		bLoop = true;
//	}
//	if (*m_pParentState == CPlayer::STATE_JUMP_RUN_LOOP2 && m_iCurMotion != CPlayer::STATE_JUMP_RUN_LOOP2)
//	{
//		m_fPlayAniTime = 1.f;
//		m_iCurMotion = CPlayer::STATE_JUMP_RUN_LOOP2;
//		bMotionChange = true;
//		bLoop = true;
//	}
//	if (*m_pParentState == CPlayer::STATE_RUN2 && m_iCurMotion != CPlayer::STATE_RUN2)
//	{
//		m_pGameInstance->PlayBGM(CSound::CHANNELID::SOUND_EFFECT_BGM, L"ST_Player_Footstep_Scuff_Sand.ogg", 0.7f);
//		m_fPlayAniTime = 1.f;
//		m_iCurMotion = CPlayer::STATE_RUN2;
//		bMotionChange = true;
//		bLoop = true;
//	}
//
//	if (*m_pParentState != CPlayer::STATE_RUN2)
//	{
//		m_pGameInstance->StopSound(CSound::CHANNELID::SOUND_EFFECT_BGM);
//	}
//
//	if (*m_pParentState != CPlayer::STATE_SPRINT2)
//	{
//		m_pGameInstance->StopSound(CSound::CHANNELID::SOUND_EFFECT_BGM2);
//	}
//
//
//	if (*m_pParentState == CPlayer::STATE_IDLE2 && m_iCurMotion != CPlayer::STATE_IDLE2)
//	{
//		m_fPlayAniTime = 1.f;
//		m_iCurMotion = CPlayer::STATE_IDLE2;
//		bMotionChange = true;
//		bLoop = true;
//	}
//
//	if (*m_pParentState == CPlayer::STATE_SWITCHIN2 && m_iCurMotion != CPlayer::STATE_SWITCHIN2)
//	{
//		m_fPlayAniTime = 1.f;
//		m_iCurMotion = CPlayer::STATE_SWITCHIN2;
//		bMotionChange = true;
//		bLoop = false;
//	}
//
//
//	if (*m_pParentState == CPlayer::STATE_STUN2 && m_iCurMotion != CPlayer::STATE_STUN2)
//	{
//		m_fPlayAniTime = 1.f;
//		m_iCurMotion = CPlayer::STATE_STUN2;
//		bMotionChange = true;
//		bLoop = false;
//	}
//
//
//
//	if (*m_pParentState == CPlayer::STATE_ASSAULTRIFlLE_RELOAD && m_iCurMotion != CPlayer::STATE_ASSAULTRIFlLE_RELOAD)
//	{
//		m_fPlayAniTime = 1.2f;
//		m_iCurMotion = CPlayer::STATE_ASSAULTRIFlLE_RELOAD;
//		bMotionChange = true;
//		bLoop = false;
//	}
//	if (*m_pParentState == CPlayer::STATE_ASSAULTRIFlLE_SHOOT && m_iCurMotion != CPlayer::STATE_ASSAULTRIFlLE_SHOOT)
//	{
//		m_fPlayAniTime = 1.9f;
//		m_iCurMotion = CPlayer::STATE_ASSAULTRIFlLE_SHOOT;
//		bMotionChange = true;
//		bLoop = false;
//	}
//
//	if (*m_pParentState == CPlayer::STATE_MissileGatling_RELOAD && m_iCurMotion != CPlayer::STATE_MissileGatling_RELOAD)
//	{
//		m_fPlayAniTime = 1.2f;
//		m_iCurMotion = CPlayer::STATE_MissileGatling_RELOAD;
//		bMotionChange = true;
//		bLoop = false;
//	}
//	if (*m_pParentState == CPlayer::STATE_MissileGatling_SHOOT && m_iCurMotion != CPlayer::STATE_MissileGatling_SHOOT)
//	{
//		m_fPlayAniTime = 1.9f;
//		m_iCurMotion = CPlayer::STATE_MissileGatling_SHOOT;
//		bMotionChange = true;
//		bLoop = false;
//	}
//	if (*m_pParentState == CPlayer::STATE_HEAVYCROSSBOW_HENDPOSE && m_iCurMotion != CPlayer::STATE_HEAVYCROSSBOW_HENDPOSE)
//	{
//		m_fPlayAniTime = 1.f;
//		m_iCurMotion = CPlayer::STATE_HEAVYCROSSBOW_HENDPOSE;
//		bMotionChange = true;
//		bLoop = false;
//	}
//	if (*m_pParentState == CPlayer::STATE_HEAVYCROSSBOW_RELOAD && m_iCurMotion != CPlayer::STATE_HEAVYCROSSBOW_RELOAD)
//	{
//		m_fPlayAniTime = 1.f;
//		m_iCurMotion = CPlayer::STATE_HEAVYCROSSBOW_RELOAD;
//		bMotionChange = true;
//		bLoop = false;
//	}
//	if (*m_pParentState == CPlayer::STATE_HEAVYCROSSBOW_SHOOT && m_iCurMotion != CPlayer::STATE_HEAVYCROSSBOW_SHOOT)
//	{
//		m_fPlayAniTime = 1.f;
//		m_iCurMotion = CPlayer::STATE_HEAVYCROSSBOW_SHOOT;
//		bMotionChange = true;
//		bLoop = false;
//	}
//
//
//	if (bMotionChange)
//		m_pModelCom->Set_Animation(m_iCurMotion, bLoop);
//
//	if (true == m_pModelCom->Play_Animation(fTimeDelta* m_fPlayAniTime ))
//	{
//		m_bFinishAni = true;
//		static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_HitLock(false);
//		static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_State(CPlayer::STATE_IDLE2);
//		m_iCurMotion = CPlayer::STATE_IDLE2;
//
//		m_pModelCom->Set_Animation(m_iCurMotion, true);
//	}else
//		m_bFinishAni = false;
//
//
//}

HRESULT CBody_Player::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
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

	return S_OK;
}

CBody_Player* CBody_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Player* pInstance = new CBody_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Player::Clone(void* pArg)
{
	CBody_Player* pInstance = new CBody_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Player::Free()
{
	__super::Free();
}