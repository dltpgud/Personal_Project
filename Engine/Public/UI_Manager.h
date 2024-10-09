#pragma once

#include "Base.h"
/*전반적인 UI 상태를 관리한다.*/

BEGIN(Engine)

class CUI_Manager final : public CBase
{
private:
    CUI_Manager();
    virtual ~CUI_Manager() = default;

public:
    HRESULT Initialize(_uint iNumLevels);
    /*원본을 추가한다*/
    HRESULT Add_UI(class CGameObject* pPrototype, _uint iLevelIndex);

    void Priority_Update(_float fTimeDelta);
    void Update(_float fTimeDelta);
    void Late_Update(_float fTimeDelta);

    /*씬 파괴를 위한 함수*/
    void Clear(_uint iClearLevelID);

    /*UI랜더 상태를 설정한다.*/
    HRESULT Set_OpenUI(const _uint& uid, _bool open);
    HRESULT Set_PreUpdateUI(const _uint& uid, _bool open);
    HRESULT Set_UpdateUI(const _uint& uid, _bool open);
    HRESULT Set_LateUpdateUI(const _uint& uid, _bool open);


    CGameObject* Get_UI(const _uint& iLevel, const _uint& uID);
    HRESULT Set_UI_shaking(const _uint& uID, _float fShakingTime, _float fPowerX, _float fPowerY );
    HRESULT UI_shaking(const _uint& uID,_float fTimeDelta);
    vector<class CUI*>* m_UIVec = {nullptr};
    _uint m_iLevel{};

public:
    static CUI_Manager* Create(_uint iNumLevels);
    virtual void Free() override;
};

END