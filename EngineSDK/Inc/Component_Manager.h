#pragma once
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "Navigation.h"
#include "Model.h"
/* 보관하는역활. */
/* 컴포넌트 원형을 보관한다. 사본은 클라에서 구현할 꺼니까 */
/* 컴포넌트 원형은 객체 원형과 달리 덩치가 크다. 레벨별로 구분하여 저장할께. */

BEGIN(Engine)
class CComponent_Manager final : public CBase
{
private:
    CComponent_Manager();
    virtual ~CComponent_Manager() = default;

public:
    HRESULT Initialize(_uint iNumLevels);

    /*원형을 추가한다*/
    HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);

    /*컴포넌트를 클라의 게임 오브젝트에서 복사할 때 호출*/
    class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);

    void Clear(_uint iLevelindex);

    map<const _wstring, class CComponent*> Get_Com_proto_vec(_uint iLevelindex);

private:
    _uint m_iNumLevels = {0};
    map<const _wstring, class CComponent*>* m_pPrototypes = {nullptr};
    typedef map<const _wstring, class CComponent*> PROTOTYPES;

private:
    /*키 값이 중복 되었는지 확인하는 함수, 맴버함 수 Add_Prototype 안에서 호출해 확인하는 용도이다 */
    class CComponent* Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);

public:
    static CComponent_Manager* Create(_uint iNumLevels);
    virtual void Free() override;
};
END
