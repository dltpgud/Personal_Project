#pragma once

#include "Base.h"

/* 현재 화면에 보여줘야할 레벨의 주소를 들고 있는다. */
/* 활성화된 레벨의 반복적인 업데이트 렌더호출의 역활. */
/* 레벨 교체의 기능을 수행한다.(주소교체 & 기존레벨 삭제 & 기존레벨용 자원을 파괴)*/

BEGIN(Engine)
class CGameObject;
class CLevel_Manager final : public CBase
{
private:
			CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Open_Level(_uint iCurrentLevelID, class CLevel* pNewLevel);
	void	Update(_float fTimeDelta);
	HRESULT Render();
	

	HRESULT Load_to_Next_Map_terrain(_uint iLevelIndex, const _uint& strLayerTag, CGameObject* ProtoObj, const _tchar* strProtoMapPath = nullptr, void* Arg = nullptr);
	HRESULT Load_to_Next_Map_NonaniObj(_uint iLevelIndex, const _uint& strLayerTag, CGameObject* ProtoObj, const _tchar* strProtoMapPath = nullptr, void* Arg = nullptr);
	HRESULT Load_to_Next_Map_Wall(_uint iLevelIndex, const _uint& strLayerTag, CGameObject* ProtoObj, const _tchar* strProtoMapPath = nullptr, void* Arg = nullptr);
	HRESULT Load_to_Next_Map_AniOBj(_uint iLevelIndex, const _uint& strLayerTag, CGameObject* ProtoObj, const _tchar* strProtoMapPath = nullptr, void* Arg = nullptr);
	/*현재 게임에 보여줄 레벨객체의 주소를 저장한다. */
private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	class CLevel*			m_pCurrentLevel = { nullptr };
	_uint					m_iCurrentLevelID = {};

public:
	static CLevel_Manager* Create();
	virtual void		   Free() override;
};

END