#pragma once

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CTexture final : public CComponent
{
private:
    CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CTexture(const CTexture& Prototype);
    virtual ~CTexture() = default;

public:
    virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
    virtual HRESULT Initialize(void* pArg) override;

public:
    HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);

private:
    /* 쉐이더에 지정한 텍스쳐를 전달한다. */
    _uint m_iNumTextures = {0};
    vector<ID3D11ShaderResourceView*> m_SRVs;

public:
    static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath,
                            _uint iNumTextures = 1);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};
END

    /* 화면에 출력해주기위한 : */
    /* -> 텍스쳐를 올려놓기위한 정점, 인덱스 버퍼 준비 */
    /* -> 렌더링파이프라인을 거친면 */
    /* -> 화면에 정점버퍼로부터 파생된 픽셀이 출력(DrawIndexed)되는거다. */
    /* -> 픽셀의 색을 결정한다.픽셀의 저장하고있는 텍스쳐라는 개념이 필요하다. */
    /* -> 이 텍스쳐를 쉐이더로 던지고 쉐이더내에서 픽셀당 색을 텍스쳐로부터 얻어와서(샘플링)
              픽셀의 색을 결정해주는 것이다. */

    /* 결론적으로 쉐이더에 던져주기위한 텍스쳐가 필요해. */
    /* ID3D11Texture2D -> ID3D11ShaderResourceView 라는 타입의 텍스쳐를 생성한다. */
    /* 내가 화면에 출력해주기위한 텍스쳐들()을 여러개 들고 있는 클래스다. */