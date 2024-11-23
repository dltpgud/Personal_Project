#pragma once

namespace Engine
{
	typedef struct ENGINE_DESC
	{
		HINSTANCE		hInstance;
		HWND			hWnd;
		_uint			iWinSizeX, iWinSizeY;
		_bool			isWindowed;
		_uint			iNumLevels;

	}ENGINE_DESC;

	typedef struct LIGHT_DESC
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOT };

		TYPE		eType;
		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;

		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;

	}LIGHT_DESC;


	typedef struct RIM_LIGHT_DESC
	{
		enum STATE { STATE_NORIM, STATE_RIM };

		const _uint*		eState;
		XMFLOAT4			fcolor;
		_int				iPower;
		XMFLOAT4			vPosition;
	}RIM_LIGHT_DESC;



	typedef struct  ENGINE_DLL VTXPOS
	{
		/* 정점의 위치 (Position)*/
		XMFLOAT3		vPosition;

		static const unsigned int		iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[1];
	}VTXPOS;

	typedef struct MATERIAL
	{
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
		XMFLOAT4	vEmissive;

	}MATERIAL;


	typedef struct ENGINE_DLL VTXPOSTEX
	{

		/*XMFLOAT 구조체는 대입연산만 가능, 덧셈,뺄쎔 등 다른 연산이 불가..*/
		/* 정점의 위치 (Position)*/
		XMFLOAT3		vPosition;

		/* 텍스쳐의 색을 가져오기위한 좌표.(Texcoord) */
		XMFLOAT2		vTexcoord;

		static const unsigned int		iNumElements = 2;  /*헤더에서 static전역 변수를 초기화하기 위해서 const 사용*/
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOSTEX;


	typedef struct ENGINE_DLL VTXPOINT
	{
		/* 정점의 위치 (Position)*/
		XMFLOAT3		vPosition;
		/* 텍스쳐의 색을 가져오기위한 좌표.(Texcoord) */
		XMFLOAT2		vPSize;

		static const unsigned int		iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXPOINT;

	typedef struct ENGINE_DLL VTXCUBE
	{
		/* 정점의 위치 (Position)*/
		XMFLOAT3		vPosition;
		/* 텍스쳐의 색을 가져오기위한 좌표.(Texcoord) */
		XMFLOAT3		vTexcoord;

		static const unsigned int		iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXCUBE;

	typedef struct ENGINE_DLL VTXNORTEX
	{

	
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int		iNumElements = 3;  /*헤더에서 static전역 변수를 초기화하기 위해서 const 사용*/
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXNORTEX;


	typedef struct ENGINE_DLL VTXMESH
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;


		XMFLOAT3		vTangent;

		static const unsigned int		iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXMESH;


	typedef struct VTXMATRIX
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;
		XMFLOAT2		vLifeTime;
	}VTXMATRIX;

	typedef struct ENGINE_DLL VTXANIMMESH
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		/* 이 정점이 영향을 받아야할 뼈들의 인덱스 */
		XMUINT4			vBlendIndex;

		/* 영향을 받아야하는 뼈들의 가중치. 뼈가 얼마나 정점들에게 영향을 미치는가*/
		XMFLOAT4		vBlendWeight;

		static const unsigned int		iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXANIMMESH;

	typedef struct KEYFRAME
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vPosition;
		float			fTrackPosition;
	}KEYFRAME;


	typedef struct ENGINE_DLL VTXPARTICLE_RECT
	{
		static const unsigned int		iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPARTICLE_RECT;

	typedef struct ENGINE_DLL VTXPARTICLE_POINT
	{
		static const unsigned int		iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPARTICLE_POINT;
}
