#pragma once
#include "Base.h"
/* 오브젝트 매니져가 게임내에 필요한 객체들을 가지고 있고 객체들의 업데이트 렌더를 모두 수행했었지만,
이제는 렌더의 기능을 렌더러로 따로 보관관리한다. */
/* 오브젝트 매니져가 가지고 있는 객체들은 사용자가 필요하따라서 그룹으로 묶어서 관리하고 있기때문에 더해서 렌더순서까지
 * 관리해주기에는 버겁다. */
/* 객체들의 렌더 순서를 관리하여 정해진 순서대로 그릴 수 있도록 하기위해 렌더러 클래스를 제작하여 사용한다. */

/* 게임내에 그려져야할 객체들의 내가 정해놓은 그룹별 순서에 의해서 렌더함수를 호출해준다. */
/* 3D에서는 깊이 테스트에 의해 그려져야할 픽셀들의 차폐에 대한 관리는 수행이된다.(NONBLEND)  */
/* 깊이 테스트라는 개념때문에 생기는 문제가 있다. 스카이박스, 유아이, 블렌드 */

/* 스카이박스 : 스카이박스는 모두에게 덮여서 그려져야한다. 스카이박스의 픽셀 당 깊이를 다른 객체들의 깊이와 비교 못하게
 * 해야할 필요가 있었다. */
/* 2가지 : 1번째 방법 스카이박스를 가장 마지막에그리면서 깊이 비교를 끄는 방법.깊이비교를 끄고 그리면 무조건 덮고
 * 그린다. => 하늘이 다른애들을 다 덮는다. (X) */
/* 2가지 : 2번째 방법 스카이박스를 가장 먼저 그리면서 깊이를 기록해놓지 않는 방법 (O) */
/* 블렌드 : 모든 불투명한 애들 다음에 그린다. 블렌드(이미 그려져있던 픽셀의 색과 그릴려고하는 픽셀의 색을 섞는다.)그룹의
 * 객체들을 그리기전에 모든 불투명한 객체들을 그려놨어야 잘 섞인다.  */
/* UI : 모든 픽셀을 다 덮고 그려낸다. 가급적 가장 마지막에 그려내되 깊이비교를 끄고 그린다. 삼차원공간에 있는 유아이다?
 * 깊이테스트가 필요해질수 있어. 논블렌드 그룹으로 가서 그려지면된다. */

BEGIN(Engine)
class CRenderer final : public CBase
{
public:
    enum RENDERGROUP : _uint
    {
        RG_PRIORITY,
        RG_SHADOW,
        RG_NONBLEND,
        RG_NONLIGHT,
        RG_BLEND,
        RG_UI,
        RG_END
    };

private:
    CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CRenderer() = default;

public:
    HRESULT Initialize();
    HRESULT Add_RenderGameObject(RENDERGROUP eRenderGroup, class CGameObject* pRenderGameObject);
    HRESULT Draw();

private:
    class CGameInstance* m_pGameInstance = {nullptr};
    ID3D11Device* m_pDevice = {nullptr};
    ID3D11DeviceContext* m_pContext = {nullptr};


    class CVIBuffer_Rect* m_pVIBuffer = { nullptr };
    class CShader* m_pShader = { nullptr };

    _float4x4					m_ViewMatrix{}, m_ProjMatrix{};

private:
    list<class CGameObject*> m_RenderGameObjects[RG_END];

private:
    HRESULT Render_Priority();
    HRESULT Render_Shadow();
    HRESULT Render_NonBlend();
    HRESULT Render_NonLight();
    HRESULT Render_Blend();
    HRESULT Render_UI();

#ifdef _DEBUG
private:
   HRESULT Render_Debug();
#endif

public:
    static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

END