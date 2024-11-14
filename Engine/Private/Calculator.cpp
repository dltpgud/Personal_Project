#include "Calculator.h"
#include "VIBuffer_Terrain.h"
#include "Transform.h"
#include "Model.h"
#include "Navigation.h"
#include "GameInstance.h"
CCalculator::CCalculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{pDevice}, m_pContext{pContext}, m_pGameInstance{ CGameInstance::GetInstance() }

{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

void CCalculator::Make_Ray(_matrix Proj, _matrix view, _vector* RayPos, _vector* RaDir, _bool forPlayer )
{   
    _uint iNumViewports = {1};
       D3D11_VIEWPORT ViewportDesc{};
 _float3 vMousePos;
    if (false == forPlayer)
    {
        POINT ptMouse{};
        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

        // 뷰 포트 -> 투영
        vMousePos.x = ptMouse.x / (ViewportDesc.Width * 0.5f) - 1.f;
        vMousePos.y = ptMouse.y / -(ViewportDesc.Height * 0.5f) + 1.f;
        vMousePos.z = 0.f;
    }
    else if (true == forPlayer)
    { 
         POINT ptPlayerAim{};
                 ptPlayerAim.x = 640;
                 ptPlayerAim.y = 360;

         m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

        // 뷰 포트 -> 투영
         vMousePos.x = ptPlayerAim.x / (ViewportDesc.Width * 0.5f) - 1.f;
         vMousePos.y = ptPlayerAim.y / -(ViewportDesc.Height * 0.5f) + 1.f;
         vMousePos.z = 0.f;
    }


    // 투영 -> 뷰 스페이스
    _float4x4 matProj;
    XMStoreFloat4x4(&matProj, Proj);
    _float4x4 InverseProj;
    XMStoreFloat4x4(&InverseProj, XMMatrixInverse(nullptr, XMLoadFloat4x4(&matProj)));
    _matrix ProjInverse = XMLoadFloat4x4(&InverseProj);

    XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), ProjInverse));

    // 뷰 스페이스 -> 월드
    _vector vRayDir, vRayPos;
    _vector Mouse = XMLoadFloat3(&vMousePos);

    vRayPos = {0.f, 0.f, 0.f, 1.f};
    vRayDir = Mouse - vRayPos;

    _matrix InverseViw = XMMatrixInverse(nullptr, view);

    _vector Pos = XMVector3TransformCoord(vRayPos, InverseViw);
    _vector Dir = XMVector3TransformNormal(vRayDir, InverseViw);

    Dir = XMVector3Normalize(Dir);

    *RayPos = Pos;
    *RaDir = Dir;

    return;
}

_float3 CCalculator::Picking_OnTerrain(HWND hWnd, CVIBuffer_Terrain* pTerrainBufferCom, _vector RayPos, _vector RayDir,
                                       CTransform* Transform, _float* fDis)
{
    _matrix matWorld = Transform->Get_WorldMatrix_Inverse();
    _vector RPos = XMVector3TransformCoord(RayPos, matWorld);
    _vector RDIR = XMVector3TransformNormal(RayDir, matWorld);

    RDIR = XMVector3Normalize(RDIR);

    const _float3* pTerrainVtx = pTerrainBufferCom->Get_VtxPos();
    const _ulong& dwCntZ = pTerrainBufferCom->Get_NumVerticesZ();
    const _ulong& dwCntX = pTerrainBufferCom->Get_NumVerticesX();
    
    _ulong dwVtxIdx[3]{};

    for (_ulong i = 0; i < dwCntZ - 1; ++i)
    {
        for (_ulong j = 0; j < dwCntX - 1; ++j)
        {
            _ulong dwIndex = i * dwCntX + j;

            // 오른쪽 위
            dwVtxIdx[0] = dwIndex + dwCntX;
            dwVtxIdx[1] = dwIndex + dwCntX + 1;
            dwVtxIdx[2] = dwIndex + 1;

            _float3 v0 = pTerrainVtx[dwVtxIdx[0]];
            _float3 v1 = pTerrainVtx[dwVtxIdx[1]];
            _float3 v2 = pTerrainVtx[dwVtxIdx[2]];

            _float fDist{};

            // V1 + U(V2 - V1) + V(V3 - V1)     FXMVECTOR Origin, FXMVECTOR Direction, FXMVECTOR V0, GXMVECTOR V1,
            // HXMVECTOR V2, float& Dist
            if (DirectX::TriangleTests::Intersects(RPos, RDIR, XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2),
                                                   fDist))
            {
                _float3 Last_pos{};
                _vector pos = RPos + RDIR * fDist;
                *fDis = fDist;
                XMStoreFloat3(&Last_pos, XMVector3TransformCoord(pos, Transform->Get_WorldMatrix()));

                return Last_pos;
            }

            // 왼쪽 아래
            dwVtxIdx[0] = dwIndex + dwCntX;
            dwVtxIdx[1] = dwIndex + 1;
            dwVtxIdx[2] = dwIndex;

            v0 = pTerrainVtx[dwVtxIdx[0]];
            v1 = pTerrainVtx[dwVtxIdx[1]];
            v2 = pTerrainVtx[dwVtxIdx[2]];
            // V1 + U(V2 - V1) + V(V3 - V1)

             if (DirectX::TriangleTests::Intersects(RPos, RDIR, XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2),
                                                   fDist))
            {
                _float3 Las_pos{};
                _vector pos = RPos + RDIR * fDist;
                *fDis = fDist;
                XMStoreFloat3(&Las_pos, XMVector3TransformCoord(pos, Transform->Get_WorldMatrix()));

                return Las_pos;
            }
        }
    }

    return _float3(0xffff,0xffff,0xffff);
}

HRESULT CCalculator::Initialize(HWND hWnd, _uint iViewportWidth, _uint iViewportHeight)
{

    g_hWnd = hWnd;


    m_iViewportWidth = iViewportWidth;
    m_iViewportHeight = iViewportHeight;

    D3D11_TEXTURE2D_DESC			TextureDesc{};

    TextureDesc.Width = iViewportWidth;
    TextureDesc.Height = iViewportHeight;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_STAGING;
    TextureDesc.BindFlags = 0;
    TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCalculator::Compute_Y(CNavigation* pNavigation, CTransform* Transform, _float3* Pos)
{
   _float3 fPos{};
   XMStoreFloat3(&fPos, Transform->Get_TRANSFORM(CTransform::TRANSFORM_POSITION));
    _float fY =  pNavigation->Compute_HeightOnCell(&fPos);
    

    *Pos = { fPos.x, fY, fPos.z };


    return S_OK;
}

_vector CCalculator::PointNomal(_float3 fP1, _float3 fP2, _float3 fP3)
{
    _vector v1 = XMLoadFloat3(&fP1);
    _vector v2 = XMLoadFloat3(&fP2);
    _vector v3 = XMLoadFloat3(&fP3);

    // 벡터 v1 -> v2와 v1 -> v3 차이 계산
    _vector edge1 = v2 - v1;
    _vector edge2 = v3 - v1;

    // 두 벡터의 외적이 평면의 법선 벡터
    return XMVector3Cross(edge1, edge2);

}

_float CCalculator::Compute_Random_Normal()
{
    return rand() / (_float)RAND_MAX;
}

_float CCalculator::Compute_Random(_float fMin, _float fMax)
{
    return (fMax - fMin) * Compute_Random_Normal() + fMin;
}

_bool CCalculator::isPicked(_float3* pOut, _bool IsPlayer)
{


    POINT			ptMouse{};

    if (false == IsPlayer) {
        GetCursorPos(&ptMouse);

        /* 뷰포트 상의 마우스 위치를 구했다. */
        ScreenToClient(g_hWnd, &ptMouse);
    }
    if (true == IsPlayer)
    {
        ptMouse.x = m_iViewportWidth * 0.5f;
        ptMouse.y = m_iViewportHeight * 0.5f;
    }


    _uint			iIndex = ptMouse.y * m_iViewportWidth + ptMouse.x;

    m_pGameInstance->Copy_RT_Resource(TEXT("Target_PickDepth"), m_pTexture2D);

    D3D11_MAPPED_SUBRESOURCE		SubResource{};

    m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);

    _float4* pPixel = static_cast<_float4*>(SubResource.pData) + iIndex;

    _float3			vWorldPos = {};

    /* 투영공간상의 위치를 구한다. */
    vWorldPos.x = ptMouse.x / (m_iViewportWidth * 0.5f) - 1.f;
    vWorldPos.y = ptMouse.y / (m_iViewportHeight * -0.5f) + 1.f;
    vWorldPos.z = pPixel->x;

    /* 뷰공간상의 위치를 구한다. */
    _vector			vPosition = XMVector3TransformCoord(XMLoadFloat3(&vWorldPos), m_pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ));


    /* 월드공간상의 위치를 구한다. */
    vPosition = XMVector3TransformCoord(vPosition, m_pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW));

    m_pContext->Unmap(m_pTexture2D, 0);

    XMStoreFloat3(pOut, vPosition);

    return _bool(pPixel->w);
}

CCalculator* CCalculator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,  HWND hWnd, _uint iViewportWidth, _uint iViewportHeight )
{
    CCalculator* pInstance = new CCalculator(pDevice, pContext);

    if (FAILED(pInstance->Initialize(hWnd, iViewportWidth, iViewportHeight)))
    {
        MSG_BOX("Failed to Created : CCalculator");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCalculator::Free()
{
    __super::Free();


    Safe_Release(m_pGameInstance);

    Safe_Release(m_pTexture2D);


    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
