#pragma once

#include "Base.h"
/*전반적인 UI 상태를 관리한다.*/

BEGIN(Engine)
class CGameObject;
class CUI_Manager final : public CBase
{
public: 
    enum UIOBJECT{UI_PROTO, UI_CLONE, UI_END};
    enum UIEVENT{EVENT_SHAKING, EVENT_END};

private:
    CUI_Manager();
    virtual ~CUI_Manager() = default;

public:
    HRESULT Initialize();
    HRESULT Add_UI_To_Proto(const _wstring& strProtoTag, class CGameObject* pUI);
    HRESULT Add_UI_To_CLone(const _wstring& strCloneTag,const _wstring& strProtoTag, void* pArg = nullptr);
    class CUI* Find_Proto_UIObj(const _wstring& strProtoUITag);
    class CUI* Find_Clone_UIObj(const _wstring& strCloneTag);
   
    void Priority_Update(_float fTimeDelta);
    void Update(_float fTimeDelta);
    void Late_Update(_float fTimeDelta);
    void Delete();
    void Clear(_uint iClearLevelID);
    void ShakingEvent(_float fTimeDelta);

    HRESULT Set_OpenUI(_bool bOpen, const _wstring& strCloneTag,class CGameObject* Owner = nullptr);
    HRESULT Set_UI_shaking(const _uint& uID, _float fShakingTime, _float fPowerX, _float fPowerY );
    HRESULT UI_shaking(const _uint& uID,_float fTimeDelta);
    HRESULT ADD_UI_ShakingList(class CUI* uiobj);

private:
    map<const _wstring, class CUI*> m_UIObj[UI_END];
    list<class CUI*> m_UIEeventList[EVENT_END];

   
    USE_LOCK;

public:
    static CUI_Manager* Create();
    virtual void Free() override;
};

END