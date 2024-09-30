#include "Texture.h"
#include "Shader.h"
CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent{pDevice, pContext}
{
}

CTexture::CTexture(const CTexture& Prototype)
    : CComponent{Prototype}, m_SRVs{Prototype.m_SRVs}, m_iNumTextures{Prototype.m_iNumTextures}
{
    for (auto& pSRV : m_SRVs) Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
    m_iNumTextures = iNumTextures;

    m_SRVs.reserve(iNumTextures);

    for (size_t i = 0; i < iNumTextures; i++)
    {
        _tchar szTextureFilePath[MAX_PATH] = TEXT("");

        /* wsprintf : 지정된 버퍼에 서식이 지정된 데이터를 씁니다.
        모든 인수는 형식 문자열의 해당 형식 사양에 따라 변환되고 출력 버퍼로 복사됩니다.*/
        wsprintf(szTextureFilePath, pTextureFilePath, i);

        _tchar szEXT[MAX_PATH] = TEXT("");

        /* _wsplitpath_s: 경로 이름을 구성 요소로 분해합니다
        매개 변수
        path  - 전체 경로

        drive - 뒤에 콜론(:)이 붙은 드라이브 문자입니다.
                드라이브 문자가 필요하지 않은 경우 이 매개 변수를 전달할 NULL 수 있습니다.

        driveNumberOfElements - 싱글바이트 문자 또는 와이드 문자 단위의 drive 버퍼 크기입니다.
                                                        drive이 NULL이면 이 값은 0이어야 합니다.

        dir - 후행 슬래시를 포함한 디렉터리 경로입니다.

        dirNumberOfElements - 싱글바이트 문자 또는 와이드 문자 단위의 dir 버퍼 크기입니다.
                                                  dir이 NULL이면 이 값은 0이어야 합니다.

        fname - 확장명이 없는 기본 파일 이름입니다.
                    파일 이름이 필요하지 않은 경우 이 매개 변수를 전달할 NULL 수 있습니다.

        nameNumberOfElements - 싱글바이트 문자 또는 와이드 문자 단위의 fname 버퍼 크기입니다.
                                                        fname이 NULL이면 이 값은 0이어야 합니다.

        ext - 선행 마침표(.)를 포함한 파일 이름 확장명입니다.
                 파일 이름 확장 프로그램이 필요하지 않은 경우 이 매개 변수를 전달할 NULL 수 있습니다.

        extNumberOfElements - 싱글바이트 문자 또는 와이드 문자 단위의 ext 버퍼 크기입니다.
                                                        ext이 NULL이면 이 값은 0이어야 합니다.
        */
        _wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

        HRESULT hr = {};

        ID3D11ShaderResourceView* pSRV = {nullptr};

        if (false == lstrcmpW(szEXT, TEXT(".dds")))
            hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

        else if (false == lstrcmpW(szEXT, TEXT(".tga")))
            hr = E_FAIL;

        else
            hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

        if (FAILED(hr))
            return E_FAIL;

        m_SRVs.push_back(pSRV);
    }
    return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
    if (iTextureIndex >= m_iNumTextures)
        return E_FAIL;

    return pShader->Bind_SRV(pConstantName, m_SRVs[iTextureIndex]);
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath,
                           _uint iNumTextures)
{
    CTexture* pInstance = new CTexture(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
    {
        MSG_BOX("Failed to Created : CTexture");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
    CTexture* pInstance = new CTexture(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CTexture");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTexture::Free()
{
    __super::Free();

    for (auto& pSRV : m_SRVs) Safe_Release(pSRV);
    m_SRVs.clear();
}
