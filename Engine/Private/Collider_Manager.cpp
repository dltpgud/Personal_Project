#include "Collider_Manager.h"
#include "Actor.h"
#include "GameInstance.h"
Collider_Manager::Collider_Manager() : m_pGameInstance{CGameInstance::GetInstance()}
{
		Safe_AddRef(m_pGameInstance);
}

HRESULT Collider_Manager::Initialize(_uint Ilevel)
{
    m_MonsterList = new list <class CActor*>[Ilevel];
    m_iLevel = Ilevel;

    return S_OK;
}

HRESULT Collider_Manager::Add_Monster(_uint Ilevel, CGameObject* Monster)
{
    CActor* pMonster = static_cast<CActor*>(Monster);
    if (nullptr == pMonster)
    {
        MSG_BOX("No Monster");
        return E_FAIL;
    }

    m_MonsterList[Ilevel].push_back(pMonster);
    return S_OK;
}

HRESULT Collider_Manager::Add_Wall(_uint Ilevel, CGameObject* wall)
{
    m_Wall[Ilevel].push_back(wall);
    return S_OK;
}


void Collider_Manager::All_Collison_check()
{

    Player_To_Monster_Collison_Check();

}

HRESULT Collider_Manager::Player_To_Monster_Collison_Check()
{
	CGameObject* pPlayer = m_pGameInstance->Get_Player();
	if (false == pPlayer)
		return E_FAIL;

    for (_uint i = 0; i < m_iLevel; i++) {
        if (0 == m_MonsterList[i].size())
            continue;

        for (auto& Monster : m_MonsterList[i])
        {
            if (nullptr == Monster)
                continue;
            Monster-> Get_Collider()->Intersect(pPlayer->Get_Collider());
        }
    }
    return S_OK;
}

HRESULT Collider_Manager::Player_To_Monster_Ray_Collison_Check()
{
    _vector RayPos{}, RayDir{};

     m_pGameInstance->Make_Ray(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ), m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW), &RayPos, &RayDir);
    CActor* pPickedObj{};

    for (_uint i = 0; i < m_iLevel; i++) {
        if (0 == m_MonsterList[i].size())
            continue;

        _float fDist{};
        _float fNewDist = {0xffff};
        for (auto& Monster : m_MonsterList[i])
        {
            if (nullptr == Monster)
                continue;
            Monster->Get_Collider()->RayIntersects(RayPos, RayDir,fDist);

            if (fDist < fNewDist)
            {
                if (fDist != 0) {
                    fNewDist = fDist;
                    pPickedObj = Monster;
                }
            }
        }
    }

    if (pPickedObj)
    {
       pPickedObj->Set_bColl(true);
    }

    return S_OK;
}


void Collider_Manager::Clear(_uint Ilevel)
{
    m_MonsterList[Ilevel].clear();

}











Collider_Manager* Collider_Manager::Create(_uint Ileve)
{
    Collider_Manager* pInstance = new Collider_Manager();

    if (FAILED(pInstance->Initialize(Ileve)))
    {
        MSG_BOX("Failed to Created : Collider_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void Collider_Manager::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);

    Safe_Delete_Array(m_MonsterList);

  
}
