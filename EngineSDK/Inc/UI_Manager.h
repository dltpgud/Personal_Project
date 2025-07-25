#pragma once

#include "Base.h"
/*전반적인 UI 상태를 관리한다.*/

BEGIN(Engine)
class CGameObject;
class CUI_Manager final : public CBase
{
public: 
    enum UIOBJECT{UI_PROTO, UI_CLONE, UI_END};

private:
    CUI_Manager();
    virtual ~CUI_Manager() = default;

public:
    HRESULT Initialize();
    /*원본을 추가한다*/
    HRESULT Add_UI_To_Proto(const _wstring& strProtoTag, class CGameObject* pUI);
    HRESULT Add_UI_To_CLone(const _wstring& strCloneTag,const _wstring& strProtoTag, void* pArg = nullptr);
    class CUI* Find_Proto_UIObj(const _wstring& strProtoUITag);
    class CUI* Find_Clone_UIObj(const _wstring& strCloneTag);
    
    HRESULT Change_UI_Level(const _uint& iCurLevel)
    {
        m_iCurLevel = iCurLevel;
        return S_OK;
    }
    void Priority_Update(_float fTimeDelta);
    void Update(_float fTimeDelta);
    void Late_Update(_float fTimeDelta);
    void Delete();
    /*씬 파괴를 위한 함수*/
    void Clear(_uint iClearLevelID);

    /*UI랜더 상태를 설정한다.*/
    HRESULT Set_OpenUI(const _uint& uid, _bool open);
    HRESULT Set_OpenUI_Inverse(const _uint& Openuid, const _uint& Cloaseduid);


    HRESULT Set_UI_shaking(const _uint& uID, _float fShakingTime, _float fPowerX, _float fPowerY );
    HRESULT UI_shaking(const _uint& uID,_float fTimeDelta);
 
private:
    map<const _wstring, class CUI*> m_UIObj[UI_END];
     _uint m_iCurLevel{};
    USE_LOCK;

public:
    static CUI_Manager* Create();
    virtual void Free() override;
};

END