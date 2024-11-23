#include "Skill.h"
#include "GameInstance.h"
#include "Cell.h"
CSkill::CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{ pDevice, pContext }
{
}

CSkill::CSkill(const CSkill& Prototype) : CGameObject{ Prototype }
{
}

HRESULT CSkill::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill::Initialize(void* pArg)
{
	if(nullptr != pArg)
	{
	Skill_DESC* pDesc = static_cast<Skill_DESC*> (pArg);
	   m_vPos = pDesc->vPos;
	   m_fLifeTime = pDesc->fLifeTime;
	   m_pDamage = pDesc->fDamage;
	   m_iActorType = pDesc->iActorType;
	   m_iSkillType = pDesc->iSkillType;
	   if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
	}
	else
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (0.f == m_fLifeTime)
		m_fLifeTime = 10.f;

	return S_OK;
}

_int CSkill::Priority_Update(_float fTimeDelta)
{
	if(true == m_bDead )
		return OBJ_DEAD;

	m_fTimeSum += fTimeDelta;



	if (m_fTimeSum > m_fLifeTime)
	{
		m_bDead = true;
	}
	return OBJ_NOEVENT;
}

void CSkill::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CSkill::Late_Update(_float fTimeDelta)
{
	

	if (true == m_bMoveStop)
		m_bDead = true;


	if(m_pNavigationCom != nullptr)
m_pGameInstance->Add_DebugComponents(m_pNavigationCom);
	
   __super::Late_Update(fTimeDelta);	
}

HRESULT CSkill::Render()
{
	return S_OK;
}

_float CSkill::Get_Damage()
{
	return *m_pDamage;
}

_uint CSkill::Get_SkillType()
{
	return m_iSkillType;
}

_uint CSkill::Get_ActorType()
{
	return m_iActorType;
}

_bool CSkill::Comput_SafeZone(_fvector vPlayerPos)
{
 _vector vCurrCenter =  XMVectorSetW( XMLoadFloat3(&m_pColliderCom->Get_iCurCenter()), 1.f);

 _vector vDir = vCurrCenter - vPlayerPos;

 _float fLength = XMVectorGetX( XMVector3Length(vDir));


 if (fLength <= m_pColliderCom->Get_iCurRadius())
	 return true;


 return false;;
}


void CSkill::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);
}
