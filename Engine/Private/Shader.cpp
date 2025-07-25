#include "Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CComponent{pDevice, pContext}
{
}

CShader::CShader(const CShader& Prototype)
    : CComponent{Prototype}, m_pEffect{Prototype.m_pEffect}, m_InputLayouts{Prototype.m_InputLayouts},
      m_iNumPasses{Prototype.m_iNumPasses}
{

    Safe_AddRef(m_pEffect);
    for (auto& pInputLayout : m_InputLayouts) Safe_AddRef(pInputLayout);
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements,
                                      _uint iNumElements)
{
    _uint iHlslFlag = {0};

#ifdef _DEBUG
    iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
    /* D3DCOMPILE_DEBUG :  디버그 파일/줄/유형/기호 정보를 출력 코드에 삽입하도록 컴파일러에 지시한다
       D3DCOMPILE_SKIP_OPTIMIZATION :코드 생성 중에 최적화 단계를 건너뛰도록 컴파일러에 지시합니다*/
#else
    iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
    /*D3DCOMPILE_OPTIMIZATION_LEVEL1 : 두 번째로 낮은 최적화 수준을 사용하도록 컴파일러에 지시합니다*/
#endif

    /*쉐이더 파일을 읽어온다*/
    if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0,
                                           m_pDevice, &m_pEffect, nullptr)))
        return E_FAIL;
    /*쉐이더 파일의 Technique를 읽어온다*/
    ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0); /*패스 컬렉션,, 즉 테크닉컬*/
    if (nullptr == pTechnique)
        return E_FAIL;

    D3DX11_TECHNIQUE_DESC TechniqueDesc{}; /*효과 기술에 대해 설명한다*/

    /*쉐이더 파일의 Technique 정보를 가져온다*/
    pTechnique->GetDesc(&TechniqueDesc); /*기술 설명을 가져온다*/

    m_iNumPasses = TechniqueDesc.Passes;

    /*쉐이더 파일의 Technique안의 Pass 개수만큼 반복문을 돌면서 벡터에 inputLayout을 push한다.*/
    for (_uint i = 0; i < m_iNumPasses; i++)
    {
        ID3D11InputLayout* pInputLayout = {nullptr};

        ID3DX11EffectPass* pPass =
            pTechnique->GetPassByIndex(i); /*ID3DX11EffectPass 인터페이스는 기술 내에서 상태 할당을 캡슐화합니다.*/
        if (nullptr == pPass)
            return E_FAIL;

        D3DX11_PASS_DESC PassDesc{};

        pPass->GetDesc(&PassDesc);

        /* 입력 버퍼 데이터를 설명하는 입력 레이아웃 개체를 만듭니다.*/
        if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature,
                                                PassDesc.IAInputSignatureSize, &pInputLayout)))
            return E_FAIL;

        /* ID3DllDevice::CreateInputLayout
             plnputElementDescs: 정점 구조체를 서술하는 D3D11_INPUT__ELEMENT_DESC들의 배열.
             NumElements：그 D3D11_INPUT_ELEMENT_DESC 배열의 원소 개수.
             pShaderBytecodeWithlnputSignature：정점 세이더(입력 서명을포함한)를 컴파일해서 얻은바이트 코드를 가리키는
           포인터. 정점 세이더는 일단의 정점 성분들을 입력 매개변수들로서 받는데，그 입력 매개변수들의 형식과 개수를
           통칭해서 입력 서명이라 부름 커스팀 정점 구조체의 성분들은 반드시 정점 세이더의 적절한 입력 매개변수에
           대응되어야한다 BytecodeLength：그 바이트 코드의 크기(바이트 단위). ppInputLayout  : 생성된 입력 배치를 이
           포인터를통해서 돌려준다.
        */

        /* dx9 때와는 달리 알아서 정점의 변환및 기타 연산을 수행해주지 않는다.
            곧, 내가 직접 정점의 변환및 필요한 연산들을 수행해한다. -> Shader 수행(사용자 정의렌더링파이프라인)을 한다.
            곧, 내가 이 정점과 인덱스를 그리기위해서는 반드시 Shader가 필요하다.
            SetInputLayout => 내가 그릴려고하는 정점을 내가 만든 쉐이더에서 잘 입력받아올 수 있는가에 대한 검증

       ID3D11InputLayout*		pInputLayout = { nullptr };
       m_pDevice->CreateInputLayout(
                                        내 정점은 어떤 멤버들을 어떤 크기로 가지고 있는가? ,
                                        내 정점은 몇개의 멤버변수를 가지고 있는가?,
                                        내가 그릴때 이용하고자하는 쉐이더는 어떤 정점들을 받는가? ,
                                        쉐이더는 어떤 크기의 정점을 받고 있는가? ,
                                    즉,정점 구조체의 각 성분(필드)이 어떤 용도인지 Direct3D에게 알려 주어야 한다.
                                    이 역할을 하는 것이 입력 배치 객체(input layout).
                                    이것은 D3D11_INPUT_ELEMENT_DESC  구조체들로 이루어진  배열을 통해
       구축한다.(배열하나가 정점 구조체 원소 하나) &pInputLayout);*/

        m_InputLayouts.push_back(pInputLayout);
    }
    return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
    if (iPassIndex >= m_iNumPasses)
        return E_FAIL;

    /*입력 배치를 사용하고자 하는 장치에 묶는다*/
    m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

    ID3DX11EffectPass* pPass = m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex);
    if (nullptr == pPass)
        return E_FAIL;

    /* 쉐이더는 전역변수를 클라이언트로부터 받아올 수 있다. */
    /* Apply함수를 호출하기 전에 받아와야할 모든 값들을 받아와야한다. */
    /* Apply함수는 쉐이더에 전달할 모든 변수를 다 던지고 호출해야한다. */
    pPass->Apply(0, m_pContext);

    return S_OK;
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
    if (nullptr == m_pEffect)
        return E_FAIL;
    /* 쉐이더파일안에 정의되어있는 지정한 이름의 전역변수에 대한 핸들을 얻어온다. */
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    /*행렬로 변환, 행렬 변수를 가져온다*/
    ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
    if (nullptr == pMatrixVariable)
        return E_FAIL;

    /*세팅 후 반환..부동 소수점 행렬을 설정합니다*/
    return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(pMatrix));
    
}

HRESULT CShader::Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
    if (nullptr == m_pEffect)
        return E_FAIL;

    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    /*셰이더 리소스를 가져온다.*/
    ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
    if (nullptr == pSRVariable)
        return E_FAIL;

    return pSRVariable->SetResource(pSRV);
}

HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength)
{
    if (nullptr == m_pEffect)
        return E_FAIL;

    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);

    if (nullptr == pVariable)
        return E_FAIL;

    return pVariable->SetRawValue(pData, 0, iLength);
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices)
{
    if (nullptr == m_pEffect)
        return E_FAIL;

    /* 쉐이더파일안에 정의되어있는 지정한 이름의 전역변수에 대한 핸들을 얻어온다. */
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
    if (nullptr == pMatrixVariable)
        return E_FAIL;

    return pMatrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(pMatrices), 0, iNumMatrices);
}

HRESULT CShader::Bind_SRVArray(const _char* pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumSRVs)
{
    if (nullptr == m_pEffect)
        return E_FAIL;

    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
    if (nullptr == pSRVariable)
        return E_FAIL;

    return pSRVariable->SetResourceArray(ppSRVs, 0, iNumSRVs);
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath,
                         const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
    CShader* pInstance = new CShader(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElements)))
    {
        MSG_BOX("Failed to Created : CShader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
    CShader* pInstance = new CShader(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CShader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CShader::Free()
{
    __super::Free();

    Safe_Release(m_pEffect);

    for (auto& pInputLayout : m_InputLayouts) Safe_Release(pInputLayout);
}
