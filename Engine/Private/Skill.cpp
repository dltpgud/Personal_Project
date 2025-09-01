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
    Skill_DESC* pDesc = static_cast<Skill_DESC*> (pArg);
	   m_vPos = pDesc->vPos;
	   m_fLifeTime = pDesc->fLifeTime;
	   m_iDamage = pDesc->iDamage;
	   m_iActorType = pDesc->iActorType;
	   m_iSkillType = pDesc->iSkillType;
       m_Clolor[CSkill::COLOR::CSTART] = pDesc->fClolor[CSkill::COLOR::CSTART];
       m_Clolor[CSkill::COLOR::CEND] = pDesc->fClolor[CSkill::COLOR::CEND];
    if (FAILED(__super::Initialize(pDesc)))
		   return E_FAIL;
	
	if (0.f == m_fLifeTime)
		m_fLifeTime = 10.f;

	return S_OK;
}

void CSkill::Priority_Update(_float fTimeDelta)
{
	m_fTimeSum += fTimeDelta;
	if (m_fTimeSum > m_fLifeTime)
	{
		Dead_Rutine();
	}
}

void CSkill::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CSkill::Late_Update(_float fTimeDelta)
{
	if(m_pNavigationCom != nullptr)
     m_pGameInstance->Add_DebugComponents(m_pNavigationCom);
	
   __super::Late_Update(fTimeDelta);	
}

HRESULT CSkill::Render()
{
	return S_OK;
}

_int CSkill::Get_Damage()
{
	return m_iDamage;
}

_uint CSkill::Get_SkillType()
{
	return m_iSkillType;
}

_uint CSkill::Get_ActorType()
{
	return m_iActorType;
}

void CSkill::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);
}
