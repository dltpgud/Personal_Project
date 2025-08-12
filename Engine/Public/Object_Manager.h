#pragma once
#include "GameObject.h"
#include "Layer.h"

BEGIN(Engine)
class CObject_Manager final : public CBase
{
private:
			CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	void    Priority_Update(_float fTimeDelta);
	void    Update(_float fTimeDelta);
	void    Late_Update(_float fTimeDelta);
    void    Delete();
	void    Clear(_uint iClearLevelID);
    HRESULT Add_Clon_to_Layers(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* clone);
	
	class CGameObject*                      Find_Prototype(const _wstring& strPrototypeTag); 
	class CGameObject*                      Find_CloneGameObject(_uint iLevelIndex, const _wstring& strLayerTag, const _uint& ProtoTag);
	map<const _wstring, class CGameObject*> Get_ProtoObject_map() { return  m_Prototypes; }
	class CGameObject*                      Clone_Prototype(const _wstring& strPrototypeTag, void* pArg);

	void                                    Set_Player(const _wstring& ProtoTag, void* pArg);
    CGameObject*                            Get_Player(){return m_pPlayer;}

private:
	class CLayer*                           Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	
public: 
    _bool                                   IsGameObject(_uint iLevelIndex, const _wstring& strLayerTag);
	CGameObject::PICKEDOBJ_DESC             Pking_onMash(const _uint& Level, const _wstring& Layer, _vector RayPos, _vector RayDir);
    CGameObject*                            Recent_GameObject(_uint iLevelIndex, const _wstring& strLayerTag);
   list<class CGameObject*>                 Get_ALL_GameObject(_uint iLevelIndex, const _wstring& strLayerTag);

private:

    USE_LOCK;
	map<const _wstring, class CGameObject*>				m_Prototypes;
    map<const _wstring, class CLayer*>*                 m_pLayers = {nullptr};
	_uint												m_iNumLevels = { 0 };
    CGameObject*                                        m_pPlayer ={nullptr};

public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void			Free() override;
};

END