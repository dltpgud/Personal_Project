#include "PartObject.h"

CPartObject::CPartObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{
}

CPartObject::CPartObject(const CPartObject & Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CPartObject::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CPartObject::Initialize(void * pArg)
{
	PARTOBJECT_DESC*		pDesc = static_cast<PARTOBJECT_DESC*>(pArg);

	m_pParentMatrix = pDesc->pParentMatrix;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;	
	
	return S_OK;
}

void CPartObject::Priority_Update(_float fTimeDelta)
{
	return ;
}

void CPartObject::Update(_float fTimeDelta)
{
	if (true == m_bDeadState)
	{
		m_fDeadTimeSum += fTimeDelta;

		if (m_fDeadTimeSum >= m_fDeadTime) {
			m_fthreshold += fTimeDelta;
			if (m_fthreshold > 1.f) {
				m_fthreshold = 1.0f;  // 이미지가 완전히 나타나면 멈춤
			}
		}
	}

	__super::Update(fTimeDelta);
}

void CPartObject::Late_Update(_float fTimeDelta)
{

    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix)); 
	__super::Late_Update(fTimeDelta);
}

HRESULT CPartObject::Render()
{
	__super::Render();
	return S_OK;
}

const _float4x4* CPartObject::Get_SocketMatrix(const _char* pBoneName)
{
	return m_pModelCom->Get_BoneMatrix(pBoneName);
}

void CPartObject::Set_BoneUpdateMatrix(const _uint& iIndex, _fmatrix NewMatrix)
{
    m_pModelCom->Set_BoneUpdateMatrix(iIndex, NewMatrix);
}

void CPartObject::Rotation(CTransform* Transform)
{
    _matrix met = XMMatrixLookAtLH(
        XMLoadFloat4x4(&m_WorldMatrix).r[3],
                         Transform->Get_TRANSFORM(CTransform::T_POSITION),
                         XMVectorSet(0.f, 1.f, 0.f, 0.f));

    _matrix Wmet = XMMatrixInverse(nullptr, met);

    _float4x4 wmet{};

    XMStoreFloat4x4(&wmet, Wmet);
    XMStoreFloat4((_float4*)&m_WorldMatrix.m[0][0], XMVectorSet(wmet._11, 0.f, wmet._13, wmet._14));
    XMStoreFloat4((_float4*)&m_WorldMatrix.m[1][0], XMVectorSet(0.f, 1.f, 0.f, wmet._24));
    XMStoreFloat4((_float4*)&m_WorldMatrix.m[2][0], XMVectorSet(wmet._31, 0.f, wmet._33, wmet._34));
}

void CPartObject::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
