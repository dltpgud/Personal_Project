#include "Actor.h"
#include "GameInstance.h"
#include "Cell.h"
CActor::CActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CContainerObject{ pDevice, pContext }
{
}

CActor::CActor(const CActor& Prototype) : CContainerObject{ Prototype }
{
}

HRESULT CActor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CActor::Initialize(void* pArg)
{
  Actor_DESC* pDesc = static_cast<Actor_DESC*> (pArg);
    m_iType   = pDesc->iType;
    m_iHP     = pDesc->iHP;
    m_iMAXHP  = m_iHP;
    m_FixY    = pDesc->fFixY;
    m_bOnCell = pDesc->bOnCell;
    m_iState  = pDesc->iState;

  if (FAILED(__super::Initialize(pDesc)))
	return E_FAIL;

 return S_OK;
}

void CActor::Priority_Update(_float fTimeDelta)
{
 
    if (true == m_bOnCell && nullptr != m_pNavigationCom ) {
		_float3 fPos{};

		Height_On_Cell(&fPos);
		m_pTransformCom->Set_TRANSFORM(CTransform::T_POSITION, XMVectorSetY(Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION), m_fY));
	}

	__super::Priority_Update(fTimeDelta);
	return ;
}

void CActor::Update(_float fTimeDelta)	
{
	__super::Update(fTimeDelta);
}

void CActor::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG

    if (m_pNavigationCom)
	m_pGameInstance->Add_DebugComponents(m_pNavigationCom);
#endif 

	__super::Late_Update(fTimeDelta);
}

HRESULT CActor::Render()
{
	return S_OK;
}

void CActor::Check_Coll()
{
  if (m_iHP > 0)
  {
      HIt_Routine();
  }
  if (m_iHP <= 0)
  {
      Dead_Routine();
  }
}

void CActor::Find_CurrentCell()
{
	m_pNavigationCom->Find_CurrentCell(m_pTransformCom->Get_TRANSFORM(CTransform::T_POSITION));
}

void CActor::Height_On_Cell(_float3* fPos)
{
	m_pGameInstance->Compute_Y(m_pNavigationCom, m_pTransformCom, fPos);
	m_fY = fPos->y + m_FixY;	
}

void CActor::Clear_CNavigation(_tchar* tFPath)
{
	m_pNavigationCom->Delete_ALLCell();
	m_pNavigationCom->Load(tFPath);
}

void CActor::Set_HealthCurrentHP(_int Health) {
	
	if (IsFullHP())
		return;

	if (m_iHP + Health >= m_iMAXHP)
    {
       m_iHP = m_iMAXHP;
    }
    else
    {
        m_iHP += Health;
    }
}

void CActor::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
