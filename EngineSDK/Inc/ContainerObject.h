#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CContainerObject abstract : public CGameObject
{
public:
	typedef struct CONTAINEROBJECT_DESC : CGameObject::GAMEOBJ_DESC
	{
		_uint	iNumPartObjects;
	}CONTAINEROBJECT_DESC;

protected:
	CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CContainerObject(const CContainerObject& Prototype);
	virtual ~CContainerObject() = default;
public:
	virtual class CComponent* Find_PartObj_Component(const _wstring& strComponentTag, _uint iPartObjID = 0) ;
public:

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
    virtual HRESULT Add_PartObjects() {return S_OK;};
	
protected:
	_uint								m_iNumPartObjects = { 0 };
	vector<class CPartObject*>			m_PartObjects;

protected:
	HRESULT Add_PartObject( const _wstring& strPrototypeTag, _uint iPartObjectIndex, void* pArg = nullptr);
    void Erase_PartObj(_uint Part);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END