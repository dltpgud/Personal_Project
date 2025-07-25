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
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void	Priority_Update(_float fTimeDelta);
	void	Update(_float fTimeDelta);
	void	Late_Update(_float fTimeDelta);
        void Delete();
	CGameObject* Get_GameObject( const _uint& ProtoTag);


	/*for_EDIT*/
	_bool IsGameObject();
	CGameObject::PICKEDOBJ_DESC CLayer::Pking_onMash(_vector RayPos, _vector RayDir);
	CGameObject* Recent_GameObject();
	list<class CGameObject*> Get_GameObject_List();

private :

	list<class CGameObject*> m_GameObjects ;


public : 

	static CLayer* Create();
	virtual void Free() override;

};

END