#include "Calculator.h"
#include "VIBuffer_Terrain.h"
#include "Transform.h"
#include "Model.h"

CCalculator::CCalculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{pDevice}, m_pContext{pContext}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

void CCalculator::Make_Ray(HWND hWnd, _matrix Proj, _matrix view, _vector* RayPos, _vector* RaDir)
{
    POINT ptMouse{};
    GetCursorPos(&ptMouse);
    ScreenToClient(hWnd, &ptMouse);

    _float3 vMousePos;

    _uint iNumViewports = {1};
    D3D11_VIEWPORT ViewportDesc{};

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    // 뷰 포트 -> 투영
    vMousePos.x = ptMouse.x / (ViewportDesc.Width * 0.5f) - 1.f;
    vMousePos.y = ptMouse.y / -(ViewportDesc.Height * 0.5f) + 1.f;
    vMousePos.z = 0.f;

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

_float3 CCalculator::Picking_OnTerrain(HWND hWnd, CVIBuffer_Terrain* pTerrainBufferCom, _matrix Proj, _matrix view, CTransform* Transform)
{
    POINT ptMouse{};
    GetCursorPos(&ptMouse);
    ScreenToClient(hWnd, &ptMouse);

    _float3 vMousePos;

    _uint iNumViewports = {1};
    D3D11_VIEWPORT ViewportDesc{};

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    // 뷰 포트 -> 투영
    vMousePos.x = ptMouse.x / (ViewportDesc.Width * 0.5f) - 1.f;
    vMousePos.y = ptMouse.y / -(ViewportDesc.Height * 0.5f) + 1.f;
    vMousePos.z = 0.f;

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

    _vector RayPos = XMVector3TransformCoord(vRayPos, InverseViw);
    _vector RayDir = XMVector3TransformNormal(vRayDir, InverseViw);

    //// 월드 -> 로컬
    
     _matrix	matWorld = Transform->Get_WorldMatrix_Inverse();
    RayPos = XMVector3TransformCoord(RayPos, matWorld);
    RayDir=  XMVector3TransformNormal(RayDir, matWorld);

    const _float3* pTerrainVtx = pTerrainBufferCom->Get_VtxPos();
    const _ulong& dwCntZ = pTerrainBufferCom->Get_NumVerticesZ();
    const _ulong& dwCntX = pTerrainBufferCom->Get_NumVerticesX();

    _ulong dwVtxIdx[3]{};

    RayDir = XMVector3Normalize(RayDir);

    _float3 Last_pos{};

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
            if (DirectX::TriangleTests::Intersects(RayPos, RayDir, XMLoadFloat3(&v0), XMLoadFloat3(&v1),
                                                   XMLoadFloat3(&v2), fDist))
            {
                _vector pos = RayPos + RayDir * fDist;
                
                 XMStoreFloat3(&Last_pos, XMVector3TransformCoord(pos, Transform->Get_WorldMatrix()));

            
                cout << Last_pos.x << " " << Last_pos.y << " " << Last_pos.z << endl;
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

            if (DirectX::TriangleTests::Intersects(RayPos, RayDir, XMLoadFloat3(&v0), XMLoadFloat3(&v1),
                                                   XMLoadFloat3(&v2), fDist))
            {
                _vector pos = RayPos + RayDir * fDist;

                 XMStoreFloat3(&Last_pos, XMVector3TransformCoord(pos, Transform->Get_WorldMatrix()));
                cout << Last_pos.x << " " << Last_pos.y << " " << Last_pos.z << endl;
                return Last_pos;
            }
        }
    }

    return _float3(0.f, 0.f, 0.f);
}

_float CCalculator:: Compute_Random_Normal()
{
    return rand() / (_float)RAND_MAX;
}

_float CCalculator::Compute_Random(_float fMin, _float fMax)
{
    return (fMax - fMin) * Compute_Random_Normal() + fMin;
}


CCalculator* CCalculator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCalculator* pInstance = new CCalculator(pDevice, pContext);

    return pInstance;
}

void CCalculator::Free()
{
    __super::Free();

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
