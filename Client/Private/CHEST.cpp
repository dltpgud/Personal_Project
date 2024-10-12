#include "stdafx.h"
#include "CHEST.h"
#include "WeaPonIcon.h"
#include "GameInstance.h"

CCHEST::CCHEST(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject{pDevice, pContext}
{
}

CCHEST::CCHEST(const CCHEST& Prototype) : CGameObject{Prototype}
{
}

HRESULT CCHEST::Initialize_Prototype()
{
    /* 패킷, 파일입ㅇ출력을 통한 초기화. */

    return S_OK;
}

HRESULT CCHEST::Initialize(void* pArg)
{
    /* 추가적으로 초기화가 필요하다면 수행해준다. */
    m_DATA_TYPE = GAMEOBJ_DATA::DATA_CHEST;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;
    return S_OK;
}

_int CCHEST::Priority_Update(_float fTimeDelta)
{
    if (m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CCHEST::Update(_float fTimeDelta)
{

    _vector vPlayerPos = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vPos = m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION);

    _vector vDir = vPos - vPlayerPos;

    // vDir =	XMVector3Normalize(vDir);

    _float fLength = XMVectorGetX(XMVector3Length(vDir));

    if (fLength <= 9.f) {
        if (fLength >= 8.f) {
            m_pModelCom[ANI]->Set_Animation(1, true);
            m_bHover = true;
        }  
    }
    else if (fLength > 9.f) {
        if (fLength <= 10.f)
            m_pModelCom[ANI]->Set_Animation(0, true);
        m_bHover = false;
    }
    
    if (m_bHover) {
        if (m_pGameInstance->Get_DIKeyDown(DIK_F))
            m_pModelCom[ANI]->Set_Animation(2, false);
    }


 
    if (true == m_bOpen)
    {
        if (false == m_bIcon)
        {

            CGameObject* pGameObject = m_pGameInstance->Clone_Prototype(L"Prototype GameObject_WeaPonIcon");

            static_cast<CWeaPonIcon*>(pGameObject)->Set_WeaPone(m_pWeaPonType);
            m_pGameInstance->Add_Clon_to_Layers(LEVEL_STATIC, CGameObject::MAP, pGameObject);

            _float3 fPos{};

            XMStoreFloat3(&fPos, m_pTransformCom->Get_TRANSFORM(CTransform::TRANSFORM_POSITION));

            pGameObject->Get_Transform()->Set_TRANSFORM(CTransform::TRANSFORM_POSITION,
                                                        XMVectorSet(fPos.x, fPos.y + 1.f, fPos.z, 1.f));

            static_cast<CWeaPonIcon*>(pGameObject)->Set_PosSave(fPos.x, fPos.y + 1.f, fPos.z);
            m_bIcon = true;
        }
    }
    if (false == m_bOpen)
    {
        if (true == m_pModelCom[ANI]->Play_Animation(fTimeDelta))
        {
            m_bOpen = true;
        }
    }
}

void CCHEST::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGameObject(CRenderer::RG_NONBLEND, this)))
        return;
}

HRESULT CCHEST::Render()
{

    for (_uint i = 0; i < CHEST_END; i++)
    {
        if (m_bOpen && i == ANI)
        {
            continue;
        }

        if (false == m_bOpen && i == NONANI)
            continue;

        if (FAILED(Bind_ShaderResources(i)))
            return E_FAIL;

        _uint iNumMeshes = m_pModelCom[i]->Get_NumMeshes();

        for (_uint j = 0; j < iNumMeshes; j++)
        {
            if (FAILED(m_pModelCom[i]->Bind_Material_ShaderResource(m_pShaderCom[i], j, aiTextureType_DIFFUSE, 0,
                                                                    "g_DiffuseTexture")))
                return E_FAIL;

            /*애니용 추가*/
            if (ANI == i)
            {
                if (FAILED(m_pModelCom[ANI]->Bind_Mesh_BoneMatrices(m_pShaderCom[ANI], j, "g_BoneMatrices")))
                    return E_FAIL;
            }
            if (FAILED(m_pShaderCom[i]->Begin(0)))
                return E_FAIL;

            m_pModelCom[i]->Render(j);
        }
    }

    return S_OK;
}

void CCHEST::Set_Model(const _wstring& protoModel)
{

    if (FAILED(__super::Add_Component(LEVEL_STAGE1, protoModel, TEXT("Com_Model"),
                                      reinterpret_cast<CComponent**>(&m_pModelCom[ANI]))))
    {
        MSG_BOX("Set_Model failed");
        return;
    }
    m_pModelCom[ANI]->Set_Animation(0, true);
}


HRESULT CCHEST::Add_Components()
{

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
                                      TEXT("Com_ANIShader"), reinterpret_cast<CComponent**>(&m_pShaderCom[ANI]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
                                      TEXT("Com_NONANIShader"), reinterpret_cast<CComponent**>(&m_pShaderCom[NONANI]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STAGE1, TEXT("Proto Component ChestWeapon Model_Nonani"),
                                      TEXT("Com_NonAniModel"), reinterpret_cast<CComponent**>(&m_pModelCom[NONANI]))))
        return E_FAIL;

    return S_OK;
}

HRESULT CCHEST::Bind_ShaderResources(_int i)
{

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom[i], "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom[i]->Bind_Matrix("g_ViewMatrix",
                                            m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom[i]->Bind_Matrix("g_ProjMatrix",
                                            m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom[i]->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom[i]->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom[i]->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom[i]->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom[i]->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CCHEST* CCHEST::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCHEST* pInstance = new CCHEST(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMonster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCHEST::Clone(void* pArg)
{
    CCHEST* pInstance = new CCHEST(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CAniObj");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCHEST::Free()
{
    __super::Free();

    for (size_t i = 0; i < CHEST_END; i++)
    {
        Safe_Release(m_pModelCom[i]);
        Safe_Release(m_pShaderCom[i]);
    }
}
