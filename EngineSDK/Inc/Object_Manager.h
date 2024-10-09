#pragma once

#include "GameObject.h"
#include "Layer.h"
/* 1. 원형객체를 보관한다. */
/* 2. 원형객체를 찾아서 복제하여 사본객체를 레이어별로 구분하여 보관한다. */
/* 3. 보관하고 있는 사본 객체들의 반복적인 갱신 작업도 수행해준다. */


BEGIN(Engine)
class CObject_Manager final : public CBase
{
private:
			CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	/*원본을 추가한다*/
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

	/*씬 파괴를 위한 함수*/
	void Clear(_uint iClearLevelID);

	/*사본을 레이어에 추가 한다.*/
	HRESULT Add_Clon_to_Layers(_uint iLevelIndex, const _uint& strLayerTag, CGameObject* clone)
	{
		/*레이어를 탐색한다*/
		CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

		if (nullptr == pLayer)
		{
			/*레이어가 비어 있으면 최초 생성한다*/
			pLayer = CLayer::Create();
			if (nullptr == pLayer)
				return E_FAIL;

			/*생성된 레이어에 사본을 추가한다*/
			pLayer->Add_GameObject(clone, strLayerTag);
			m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
		}
		else /*최초 생성이 아니라면*/
			pLayer->Add_GameObject(clone, strLayerTag);
		return S_OK;
	}

	/*원본을 탐색한다*/
	class CGameObject* Find_Prototype(const _wstring& strPrototypeTag); 
	map<const _wstring, class CGameObject*> Get_ProtoObject_map() { return  m_Prototypes; }
	class CGameObject* Clone_Prototype(const _wstring& strPrototypeTag, void* pArg);

private:
	/*레이어를 탐색한다*/
	class CLayer* Find_Layer(_uint iLevelIndex, const _uint& strLayerTag);
	
public: /*편집씬을 위한 함수*/
	_bool IsGameObject(_uint iLevelIndex, const _uint& strLayerTag);
	CGameObject::PICKEDOBJ_DESC Pking_onMash(_vector RayPos, _vector RayDir);
	CGameObject* Recent_GameObject( const _uint& strLayerTag);
	list<class CGameObject*> Get_ALL_GameObject(_uint iLevelIndex, const _uint& strLayerTag);

private:
	map<const _wstring, class CGameObject*>				m_Prototypes;
	/*원형을 복제한  사본객체를 레벨별로 그룹지어 저장한다, 
	레벨의 enum 정의는 클라에 되있어서 이니셜라이즈에서 동적배열로 할당*/
	map<const _uint, class CLayer*>*					m_pLayers	 = { nullptr };
	_uint												m_iNumLevels = { 0 };

public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void			Free() override;
};

END