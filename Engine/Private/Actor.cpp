#include "Actor.h"
#include "GameInstance.h"
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
	if(nullptr != pArg)
	{
		Actor_DESC* pDesc = static_cast<Actor_DESC*> (pArg);
	
		if (FAILED(__super::Initialize(pDesc)))
			return E_FAIL;
	}
	else
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}

_int CActor::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	return OBJ_NOEVENT;
}

void CActor::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CActor::Late_Update(_float fTimeDelta)
{
	if (m_bColl)
	{

		if (m_fHP > 0.f)
		{
			HIt_Routine();
		}
		if (m_fHP <= 0.f)
		{
			Dead_Routine();
		}
                m_bColl = false;
	}

   __super::Late_Update(fTimeDelta);
}

HRESULT CActor::Render()
{
#ifdef _DEBUG

	if (m_pNavigationCom) {
		m_pNavigationCom->Render();
	}

#endif
	__super::Render();

	return S_OK;
}

void CActor::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
