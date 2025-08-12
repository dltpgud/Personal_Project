#include "Timer_Manager.h"

CTimer_Manager::CTimer_Manager(void)
{

}


_float CTimer_Manager::Get_TimeDelta(const _wstring& pTimerTag)
{
	CTimer*		pInstance = Find_Timer(pTimerTag);
	
	if (nullptr == pInstance)
		return 0.f;

	return pInstance->Get_TimeDelta();
}

HRESULT CTimer_Manager::Add_Timer(const _wstring& strTimerTag)
{
	 if (nullptr != Find_Timer(strTimerTag))
	  return E_FAIL;

	 CTimer* pTimer = CTimer::Create();
	 if (nullptr == pTimer)
	  return E_FAIL;

	 m_mapTimer.emplace(strTimerTag, pTimer);

	  return S_OK;
}

void CTimer_Manager::Update_TimeDelta(const _wstring& strTimerTag)
{
	CTimer* pInstance = Find_Timer(strTimerTag);
	if (nullptr == pInstance)
		return;

	 pInstance->Update_Timer();
}

CTimer_Manager* CTimer_Manager::Create()
{
	CTimer_Manager* pInstance = new CTimer_Manager();
	return pInstance;
}

void CTimer_Manager::Free(void)
{
	__super::Free();

	//for_each(m_mapTimer.begin(), m_mapTimer.end(), CDeleteMap());

	for (auto& pTimemgr : m_mapTimer)
		Safe_Release(pTimemgr.second);
	m_mapTimer.clear();
}

CTimer* CTimer_Manager::Find_Timer(const _wstring& strTimerTag)
{
	auto	iter = m_mapTimer.find(strTimerTag);

	if (iter == m_mapTimer.end())
		return nullptr;

	return iter->second;
}

