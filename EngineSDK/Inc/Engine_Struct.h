#pragma once

namespace Engine
{
    typedef struct ENGINE_DESC
    {
        HINSTANCE hInstance;
        HWND hWnd;
        _uint iWinSizeX, iWinSizeY;
        _bool isWindowed;
        _uint iNumLevels;
    } ENGINE_DESC;

    typedef struct LIGHT_DESC
    {
        enum TYPE
        {
            TYPE_DIRECTIONAL,
            TYPE_POINT,
            TYPE_SPOT
        };

        TYPE eType;
        XMFLOAT4 vDirection;
        XMFLOAT4 vPosition;
        float fRange;

        XMFLOAT4 vDiffuse;
        XMFLOAT4 vAmbient;
        XMFLOAT4 vSpecular;

    } LIGHT_DESC;

    struct DecalInstanceData
    {
        XMFLOAT4X4 WorldInv;
        XMFLOAT3 DecalPos; 
        XMFLOAT3 DecalDir; // SSD 용
        XMFLOAT3 HalfSize; 
        float LifeTime;
        float DecalTime;
        int TexIndex ;
        int bNormal;
        int DecalType; // SSD or Box
        int ProtoIndex; 
    };

    struct TOIResult
    {
        bool hit = false;
        float toi = 1.0f;                  // 0~1 (1이면 이번 프레임 충돌 없음)
        float distance = 0.0f;             // 실제 이동거리 기준 (선택)
        _vector position = XMVectorZero(); // 충돌 지점(월드)
        _vector normal = XMVectorZero();   // 충돌 법선(월드)
    };

    struct HitResult
    {
        _bool hit = false;
        _float distance = FLT_MAX;
        _vector position = XMVectorZero();
        _vector normal = XMVectorZero();
        class CGameObject* object = nullptr;
    };
   
    typedef struct AABB
    {
        _float3 min;
        _float3 max;

        XMVECTOR Center() const
        {
            _float3 center = {(min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f};

            // XMVECTOR로 변환하여 반환
            return XMVectorSet(center.x, center.y, center.z, 1.0f);
        }

    } AABB;

    typedef struct DISSOLVE_DESC
    {
        float fDissolve_threshold{0.f};
        float fDissolveTimeSum = {0.f};
        bool bDissolveState = {false};

        void Check_DisslveSt(float fTimeDelta)
        {

            if (true == bDissolveState)
            {
                fDissolveTimeSum += fTimeDelta;

                fDissolve_threshold += fTimeDelta;
                if (fDissolve_threshold > 1.f)
                {
                    fDissolve_threshold = 1.0f;
                }
            }
        }

    } DISSOLVE_DESC;

    typedef struct RIM_LIGHT_DESC
    {
        enum STATE
        {
            STATE_NORIM,
            STATE_RIM
        }; // RimLight 상태.

        unsigned int* eState{}; // 현재 RimLight 상태
        XMFLOAT4 fcolor;        // RimLight 색상
        float iPower;           // RimLight 세기
    } RIM_LIGHT_DESC;


    typedef struct ENGINE_DLL VTXPOS
    {
        /* 정점의 위치 (Position)*/
        XMFLOAT3 vPosition;

        static const unsigned int iNumElements = 1;
        static const D3D11_INPUT_ELEMENT_DESC Elements[1];
    } VTXPOS;

    typedef struct MATERIAL
    {
        XMFLOAT4 vDiffuse;
        XMFLOAT4 vAmbient;
        XMFLOAT4 vSpecular;
        XMFLOAT4 vEmissive;

    } MATERIAL;

    typedef struct ENGINE_DLL VTXPOSTEX
    {

        /*XMFLOAT 구조체는 대입연산만 가능, 덧셈,뺄쎔 등 다른 연산이 불가..*/
        /* 정점의 위치 (Position)*/
        XMFLOAT3 vPosition;

        /* 텍스쳐의 색을 가져오기위한 좌표.(Texcoord) */
        XMFLOAT2 vTexcoord;

        static const unsigned int iNumElements = 2; /*헤더에서 static전역 변수를 초기화하기 위해서 const 사용*/
        static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
    } VTXPOSTEX;

    typedef struct ENGINE_DLL VTXPOINT
    {
        /* 정점의 위치 (Position)*/
        XMFLOAT3 vPosition;
        /* 텍스쳐의 색을 가져오기위한 좌표.(Texcoord) */
        XMFLOAT2 vPSize;

        static const unsigned int iNumElements = 2;
        static const D3D11_INPUT_ELEMENT_DESC Elements[2];
    } VTXPOINT;

    typedef struct ENGINE_DLL VTXCUBE
    {
        /* 정점의 위치 (Position)*/
        XMFLOAT3 vPosition;
        /* 텍스쳐의 색을 가져오기위한 좌표.(Texcoord) */
        XMFLOAT3 vTexcoord;

        static const unsigned int iNumElements = 2;
        static const D3D11_INPUT_ELEMENT_DESC Elements[2];
    } VTXCUBE;

    typedef struct ENGINE_DLL VTXNORTEX
    {
        XMFLOAT3 vPosition;
        XMFLOAT3 vNormal;
        XMFLOAT2 vTexcoord;

        static const unsigned int iNumElements = 3; /*헤더에서 static전역 변수를 초기화하기 위해서 const 사용*/
        static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
    } VTXNORTEX;

    typedef struct DECAL_DESC
    {
        enum TYPE
        {
            TYPE_BOX,
            TYPE_SSD
        };

        XMVECTOR vPos{};
        XMVECTOR vDir{};
        XMVECTOR vNormal{};
        float fSize = 1.f;
        float fDepth = 1.f;
        float fLifeTime = 3.f;
        bool bNormal{true};
        int iType{};
        int iTexIndex{1};
        bool bOnce{};
        bool bActive{};
        bool bColActive{};
        float DeltaScaling{};
        _int iContinuous{0};
        wstring Key{};
    } DECAL_DESC;

   typedef struct VTXTRAIL
    {
        _float4 vHead;     // 트레일 시작점
        _float4 vTail;     // 트레일 끝점
        _float4 vColor;    // 색상
        _float2 vLifeTime; // x:총수명, y:경과
        _float2 vPad;
    } VTXTRAIL_DESC;


    typedef struct ENGINE_DLL VTXMESH
    {
        XMFLOAT3 vPosition;
        XMFLOAT3 vNormal;
        XMFLOAT2 vTexcoord;
        XMFLOAT3 vTangent;

        static const unsigned int iNumElements = 4;
        static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
    } VTXMESH;

    typedef struct VTXMATRIX
    {
        XMFLOAT4 vRight;
        XMFLOAT4 vUp;
        XMFLOAT4 vLook;
        XMFLOAT4 vTranslation;
        XMFLOAT2 vLifeTime;
    } VTXMATRIX;

    typedef struct ENGINE_DLL VTXMATRIX_INSTANCE
    {
        XMFLOAT4 vRight;
        XMFLOAT4 vUp;
        XMFLOAT4 vLook;
        XMFLOAT4 vPos;
        static const unsigned int iNumElement = 8;
        static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElement];
    } VTXMATRIX_INSTANCE;

    typedef struct ENGINE_DLL VTXANIMMESH
    {
        XMFLOAT3 vPosition;
        XMFLOAT3 vNormal;
        XMFLOAT2 vTexcoord;
        XMFLOAT3 vTangent;

        /* 이 정점이 영향을 받아야할 뼈들의 인덱스 */
        XMUINT4 vBlendIndex;

        /* 영향을 받아야하는 뼈들의 가중치. 뼈가 얼마나 정점들에게 영향을 미치는가*/
        XMFLOAT4 vBlendWeight;

        static const unsigned int iNumElements = 6;
        static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
    } VTXANIMMESH;

    typedef struct KEYFRAME
    {
        XMFLOAT3 vScale;
        XMFLOAT4 vRotation;
        XMFLOAT3 vPosition;
        float fTrackPosition;
    } KEYFRAME;

    typedef struct CASCADE_DATA
    {
        _float4x4 LightViewProj;      // 라이트 VP 행렬
        std::vector<_float3> Corners; // 월드 공간 프러스텀 꼭짓점 (8개)
    } CASCADE_DATA;

    typedef struct ENGINE_DLL VTXPARTICLE_RECT
    {
        static const unsigned int iNumElements = 7;
        static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
    } VTXPARTICLE_RECT;

    typedef struct ENGINE_DLL VTXPARTICLE_POINT
    {
        static const unsigned int iNumElements = 7;
        static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];

    } VTXPARTICLE_POINT;

    typedef struct TRAIL_POINT
    {
        XMFLOAT3 vPosition;
        XMFLOAT2 vTexcoord;

            static const unsigned int iNumElements = 4;
        static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];

    } TRAIL_POINT;
}
