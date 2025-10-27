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
    _float3 vMousePos{};
    
    // 뷰포트 정보를 직접 계산 (RSGetViewports 대신)
    _float ViewportWidth = static_cast<_float>(m_iViewportWidth);
    _float ViewportHeight = static_cast<_float>(m_iViewportHeight);

    if (false == forPlayer)
     { // 맵툴에서는 마우스 위치에 따라서 Ray를 쏘는 위치가 바뀌어야 한다.
        POINT ptMouse{};
        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        // 뷰 포트 -> 투영
        vMousePos.x = ptMouse.x / (ViewportWidth * 0.5f) - 1.f;
        vMousePos.y = ptMouse.y / -(ViewportHeight * 0.5f) + 1.f;
        vMousePos.z = 0.f;
    }
    else if (true == forPlayer)
    {  //1인칭 슈팅게임 클라이언트에서 Ray를 쏘는 위치는 고정되어있다.
         POINT ptPlayerAim{};
                 ptPlayerAim.x = 640;
                 ptPlayerAim.y = 420;

        // 뷰 포트 -> 투영
         vMousePos.x = ptPlayerAim.x / (ViewportWidth * 0.5f) - 1.f;
         vMousePos.y = ptPlayerAim.y / -(ViewportHeight * 0.5f) + 1.f;
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

_float3 CCalculator::Picking_OnTerrain(CVIBuffer_Terrain* pTerrainBufferCom, _vector RayPos, _vector RayDir,
                                       CTransform* Transform, OUT _float* fDis, OUT _float3* vNormal)
{

    _float3 vPosition = _float3(FLT_MAX, FLT_MAX, FLT_MAX);
    _float3 fNormal = _float3(0.f, 1.f, 0.f); // 기본값 ↑
    _float fDistance = FLT_MAX;

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
            if (DirectX::TriangleTests::Intersects(RPos, RDIR, XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2),
                                                   fDist))
            {
                *fDis = fDist;
                XMStoreFloat3(&vPosition,XMVector3TransformCoord(RPos + RDIR * fDist, Transform->Get_WorldMatrix()));

                if (vNormal)
                {
                    _vector e0 = XMLoadFloat3(&v1) - XMLoadFloat3(&v0);
                    _vector e1 = XMLoadFloat3(&v2) - XMLoadFloat3(&v0);
                    _vector n = XMVector3Normalize(XMVector3Cross(e0, e1));

                    n = XMVector3TransformNormal(n, Transform->Get_WorldMatrix());
                    n = XMVector3Normalize(n);
                    XMStoreFloat3(&fNormal, n);

                    *vNormal = fNormal;
                }
              return vPosition;
            }

            // 왼쪽 아래
            dwVtxIdx[0] = dwIndex + dwCntX;
            dwVtxIdx[1] = dwIndex + 1;
            dwVtxIdx[2] = dwIndex;

            v0 = pTerrainVtx[dwVtxIdx[0]];
            v1 = pTerrainVtx[dwVtxIdx[1]];
            v2 = pTerrainVtx[dwVtxIdx[2]];

            if (DirectX::TriangleTests::Intersects(RPos, RDIR, XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2), fDist))
            {
                 *fDis = fDist;
                 XMStoreFloat3(&vPosition, XMVector3TransformCoord(RPos + RDIR * fDist, Transform->Get_WorldMatrix()));
                 if (vNormal)
                 {
                     _vector e0 = XMLoadFloat3(&v1) - XMLoadFloat3(&v0);
                     _vector e1 = XMLoadFloat3(&v2) - XMLoadFloat3(&v0);
                     _vector n = XMVector3Normalize(XMVector3Cross(e0, e1));

                     n = XMVector3TransformNormal(n, Transform->Get_WorldMatrix());
                     n = XMVector3Normalize(n);
                     XMStoreFloat3(&fNormal, n);

                     *vNormal = fNormal;
                 }
                return vPosition;
            }
        }
    }

    return _float3(FLT_MAX, FLT_MAX, FLT_MAX);
}


HRESULT CCalculator::Initialize(HWND hWnd, _uint iViewportWidth, _uint iViewportHeight)
{
    g_hWnd = hWnd;
    m_iViewportWidth = iViewportWidth;
    m_iViewportHeight = iViewportHeight;
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

_bool CCalculator::RayIntersectsAABB_Local(_vector rayO_L, _vector rayD_L, const _float3& mn, const _float3& mx)
{
    _float3 o, d;
    XMStoreFloat3(&o, rayO_L);
    XMStoreFloat3(&d, rayD_L);

    _float tmin = 0.f, tmax = FLT_MAX;

    auto slab = [&](_float o, _float d, _float mn, _float mx, _float& t0, _float& t1)
    {
        if (fabsf(d) < 1e-8f)
        {
            t0 = (o < mn || o > mx) ? 1.f : 0.f;
            t1 = (o < mn || o > mx) ? 0.f : FLT_MAX;
        }
        else
        {
            float inv = 1.f / d;
            float tA = (mn - o) * inv;
            float tB = (mx - o) * inv;
            t0 = min(tA, tB);
            t1 = max(tA, tB);
        }
    };

    _float tx0, tx1, ty0, ty1, tz0, tz1;
    slab(o.x, d.x, mn.x, mx.x, tx0, tx1);
    slab(o.y, d.y, mn.y, mx.y, ty0, ty1);
    slab(o.z, d.z, mn.z, mx.z, tz0, tz1);

    tmin = max(tmin, max(tx0, max(ty0, tz0)));
    tmax = min(tmax, min(tx1, min(ty1, tz1)));
    return tmax >= tmin;
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
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
