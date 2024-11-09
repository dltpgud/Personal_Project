#pragma once
#include "Base.h"
#include "UI.h"
BEGIN(Engine)

class ENGINE_DLL CLayer  : public CBase
{
private: 
			CLayer();
	virtual ~CLayer() = default;
	
public:
	HRESULT Add_GameObject(class CGameObject* pGameObject, const _uint& strLayerTag);
	void	Priority_Update(_float fTimeDelta);
	void	Update(_float fTimeDelta);
	void	Late_Update(_float fTimeDelta);
	CGameObject* Get_GameObject(const _uint& strLayerTag, const _uint& ProtoTag);


	/*for_EDIT*/
	_bool IsGameObject(const _uint& strLayerTag);
	CGameObject::PICKEDOBJ_DESC CLayer::Pking_onMash(_vector RayPos, _vector RayDir);
	CGameObject* Recent_GameObject(const _uint& strLayerTag);
	list<class CGameObject*> Get_ALL_GameObject(const _uint& strLayerTag);

private :
    _uint					m_iObjType = CGameObject::GAMEOBJ_TYPE_END;
	list<class CGameObject*> m_GameObjects[CGameObject::GAMEOBJ_TYPE_END] ;


public : 

	static CLayer* Create();
	virtual void Free() override;

};

END