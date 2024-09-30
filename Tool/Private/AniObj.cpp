#include "stdafx.h"
#include "AniObj.h"

#include "GameInstance.h"

CAniObj::CAniObj(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{
}

CAniObj::CAniObj(const CAniObj& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CAniObj::Initialize_Prototype()
{
	/* 패킷, 파일입ㅇ출력을 통한 초기화. */

	return S_OK;
}

HRESULT CAniObj::Initialize(void * pArg)
{
	GAMEOBJ_DESC* pDesc = static_cast<GAMEOBJ_DESC*>(pArg);
	m_DATA_TYPE = pDesc->DATA_TYPE;


	_uint iLen = wcslen(pDesc->ProtoName) + 1;
	m_Proto = new wchar_t[iLen];
	lstrcpyW(m_Proto, pDesc->ProtoName);

	/* 추가적으로 초기화가 필요하다면 수행해준다. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


//	m_pTransformCom->Set_TRANSFORM(CTransform::TRANSFORM_POSITION, XMVectorSet(
	//	m_pGameInstance->Compute_Random(0.f, 1.f), 0.f, m_pGameInstance->Compute_Random(0.f, 1.f), 1.f));



	return S_OK;
}

_int CAniObj::Priority_Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CAniObj::Update(_float fTimeDelta)
{
	if (true == m_pModelCom->Play_Animation(fTimeDelta))
	_uint iData = 10;
}

void CAniObj::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CAniObj::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType_DIFFUSE, 0, "g_DiffuseTexture")))
			return E_FAIL;
		

		/*애니용 추가*/
		if (FAILED(m_pModelCom->Bind_Mesh_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
			return E_FAIL;   

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}
   
	return S_OK;
}

void CAniObj::Set_Model(const _wstring& protoModel)
{	
	m_wModel = protoModel;
		/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, protoModel,
	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	return ;

	m_pModelCom->Set_Animation(3, true);
}

CTransform* CAniObj::Get_Transform()
{
	return m_pTransformCom;
}

_tchar* CAniObj::Get_ProtoName()
{
	return m_Proto;
}

HRESULT CAniObj::Add_Components()
{
	/* 멤버변수로 직접 참조를 하게되면 */
	/* 1. 내가 내 컴포넌트를 이용하고자할 때 굳이 검색이 필요없이 특정 멤버변수로 바로 기능을 이용하면 된다. */
	/* 2. 다른 객체가 내 컴포넌트를 검색하고자 할때 스위치케이스가 겁나 늘어나는 상황. */
		
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CAniObj::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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

CAniObj* CAniObj::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAniObj*		pInstance = new CAniObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAniObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAniObj::Clone(void * pArg)
{
	CAniObj*		pInstance = new CAniObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAniObj::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	if (m_bClone)
		Safe_Delete_Array(m_Proto);
}
