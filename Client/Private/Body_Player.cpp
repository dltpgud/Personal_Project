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

	m_pParentState = pDesc->pParentState;
	m_pType = pDesc->pType;
	/* 추가적으로 초기화가 필요하다면 수행해준다. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	 return S_OK;
}

_int CBody_Player::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;


	return OBJ_NOEVENT;
}

void CBody_Player::Update(_float fTimeDelta)
{
	*m_pType == CPlayer::T00 ? Type0_Update(fTimeDelta) : Type2_Update(fTimeDelta);
}

void CBody_Player::Late_Update(_float fTimeDelta)
{

 	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pParentMatrix) * m_pTransformCom->Get_WorldMatrix());  //부모행렬과 내 월드랑 곱해서 그린다

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

void CBody_Player::Type0_Update(_float fTimeDelta)
{
	_bool bMotionChange = { false }, bLoop = { false };
	
	if (*m_pParentState == CPlayer::STATE_HIT_FRONT && m_iCurMotion != CPlayer::STATE_HIT_FRONT)
	{
		m_iCurMotion = CPlayer::STATE_HIT_FRONT;
		bMotionChange = true;
		bLoop = false;
	}
	if (*m_pParentState == CPlayer::STATE_SPRINT && m_iCurMotion != CPlayer::STATE_SPRINT)
	{
		m_iCurMotion = CPlayer::STATE_SPRINT;
		bMotionChange = true;
		bLoop = true;
	}
	if (*m_pParentState == CPlayer::STATE_JUMP_RUN_LOOP && m_iCurMotion != CPlayer::STATE_JUMP_RUN_LOOP)
	{
		m_iCurMotion = CPlayer::STATE_JUMP_RUN_LOOP;
		bMotionChange = true;
		bLoop = true;
	}
	if (*m_pParentState == CPlayer::STATE_JUMP_RUN_HIGH && m_iCurMotion != CPlayer::STATE_JUMP_RUN_HIGH)
	{
		m_iCurMotion = CPlayer::STATE_JUMP_RUN_HIGH;
		bMotionChange = true;
		bLoop = true;
	}
	if (*m_pParentState == CPlayer::STATE_RUN && m_iCurMotion != CPlayer::STATE_RUN)
	{
		m_iCurMotion = CPlayer::STATE_RUN;
		bMotionChange = true;
		bLoop = true;
	}

	if (*m_pParentState == CPlayer::STATE_SWITCHIN && m_iCurMotion != CPlayer::STATE_SWITCHIN)
	{
		m_iCurMotion = CPlayer::STATE_SWITCHIN;
		bMotionChange = true;
		bLoop = false;
	}

	if (*m_pParentState == CPlayer::STATE_STUN && m_iCurMotion != CPlayer::STATE_STUN)
	{
		m_iCurMotion = CPlayer::STATE_STUN;
		bMotionChange = true;
		bLoop = false;
	}

	if (*m_pParentState == CPlayer::STATE_IDLE && m_iCurMotion != CPlayer::STATE_IDLE)
	{
		m_iCurMotion = CPlayer::STATE_IDLE;
		bMotionChange = true;
		bLoop = true;
	}

	if (*m_pParentState == CPlayer::STATE_HENDGUN_RELOAD && m_iCurMotion != CPlayer::STATE_HENDGUN_RELOAD)
	{
		m_iCurMotion = CPlayer::STATE_HENDGUN_RELOAD;
		bMotionChange = true;
		bLoop = false;
	}
	if (*m_pParentState == CPlayer::STATE_HENDGUN_SHOOT && m_iCurMotion != CPlayer::STATE_HENDGUN_SHOOT)
	{
	 	m_iCurMotion = CPlayer::STATE_HENDGUN_SHOOT;
		bMotionChange = true;
		bLoop = false;
	}

	if (bMotionChange)
		m_pModelCom->Set_Animation(m_iCurMotion, bLoop);

	if (true == m_pModelCom->Play_Animation(fTimeDelta))
	{
		static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_State(CPlayer::STATE_IDLE);
		m_iCurMotion = CPlayer::STATE_IDLE;
		m_pModelCom->Set_Animation(m_iCurMotion, true);
	}
}

void CBody_Player::Type2_Update(_float fTimeDelta)
{
	_bool bMotionChange = { false }, bLoop = { false };

	if (*m_pParentState == CPlayer::STATE_JUMP_RUN_HIGH2 && m_iCurMotion != CPlayer::STATE_JUMP_RUN_HIGH2)
	{
		m_iCurMotion = CPlayer::STATE_JUMP_RUN_HIGH2;
		bMotionChange = true;
		bLoop = true;
	}

	if (*m_pParentState == CPlayer::STATE_SPRINT2 && m_iCurMotion != CPlayer::STATE_SPRINT2)
	{
		m_iCurMotion = CPlayer::STATE_SPRINT2;
		bMotionChange = true;
		bLoop = true;
	}
	if (*m_pParentState == CPlayer::STATE_JUMP_RUN_LOOP2 && m_iCurMotion != CPlayer::STATE_JUMP_RUN_LOOP2)
	{
		m_iCurMotion = CPlayer::STATE_JUMP_RUN_LOOP2;
		bMotionChange = true;
		bLoop = true;
	}
	if (*m_pParentState == CPlayer::STATE_RUN2 && m_iCurMotion != CPlayer::STATE_RUN2)
	{
		m_iCurMotion = CPlayer::STATE_RUN2;
		bMotionChange = true;
		bLoop = true;
	}

	if (*m_pParentState == CPlayer::STATE_IDLE2 && m_iCurMotion != CPlayer::STATE_IDLE2)
	{
		m_iCurMotion = CPlayer::STATE_IDLE2;
		bMotionChange = true;
		bLoop = true;
	}

	if (*m_pParentState == CPlayer::STATE_SWITCHIN2&& m_iCurMotion != CPlayer::STATE_SWITCHIN2)
	{
		m_iCurMotion = CPlayer::STATE_SWITCHIN2;
		bMotionChange = true;
		bLoop = false;
	}

	if (*m_pParentState == CPlayer::STATE_HIT_FRONT2 && m_iCurMotion != CPlayer::STATE_HIT_FRONT2)
	{
		m_iCurMotion = CPlayer::STATE_HIT_FRONT2;
		bMotionChange = true;
		bLoop = false;
	}

	if (*m_pParentState == CPlayer::STATE_ASSAULTRIFlLE_RELOAD && m_iCurMotion != CPlayer::STATE_ASSAULTRIFlLE_RELOAD)
	{
		m_iCurMotion = CPlayer::STATE_ASSAULTRIFlLE_RELOAD;
		bMotionChange = true;
		bLoop = false;
	}
	if (*m_pParentState == CPlayer::STATE_ASSAULTRIFlLE_SHOOT && m_iCurMotion != CPlayer::STATE_ASSAULTRIFlLE_SHOOT)
	{
		m_iCurMotion = CPlayer::STATE_ASSAULTRIFlLE_SHOOT;
		bMotionChange = true;
		bLoop = false;
	}

	if (*m_pParentState == CPlayer::STATE_MissileGatling_RELOAD && m_iCurMotion != CPlayer::STATE_MissileGatling_RELOAD)
	{
		m_iCurMotion = CPlayer::STATE_MissileGatling_RELOAD;
		bMotionChange = true;
		bLoop = false;
	}
	if (*m_pParentState == CPlayer::STATE_MissileGatling_SHOOT && m_iCurMotion != CPlayer::STATE_MissileGatling_SHOOT)
	{
		m_iCurMotion = CPlayer::STATE_MissileGatling_SHOOT;
		bMotionChange = true;
		bLoop = false;
	}

	if (*m_pParentState == CPlayer::STATE_HEAVYCROSSBOW_RELOAD && m_iCurMotion != CPlayer::STATE_HEAVYCROSSBOW_RELOAD)
	{
		m_iCurMotion = CPlayer::STATE_HEAVYCROSSBOW_RELOAD;
	 	bMotionChange = true;
	 	bLoop = false;
	}
	if (*m_pParentState == CPlayer::STATE_HEAVYCROSSBOW_SHOOT && m_iCurMotion != CPlayer::STATE_HEAVYCROSSBOW_SHOOT)
	{
		m_iCurMotion = CPlayer::STATE_HEAVYCROSSBOW_SHOOT;
		bMotionChange = true;
		bLoop = false;
	}
	

	if (bMotionChange)
		m_pModelCom->Set_Animation(m_iCurMotion, bLoop);

	if (true == m_pModelCom->Play_Animation(fTimeDelta))
	{
		static_cast<CPlayer*>(m_pGameInstance->Get_Player())->Set_State(CPlayer::STATE_IDLE2);
		m_iCurMotion = CPlayer::STATE_IDLE2;
		m_pModelCom->Set_Animation(m_iCurMotion, true);
	}
}

const _float4x4* CBody_Player::Get_SocketMatrix(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrix(pBoneName);
}

HRESULT CBody_Player::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
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

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
