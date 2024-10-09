#include "Engine_Defines.h"

namespace Engine
{
	typedef struct tagFrame
	{
		int iFrameStart;
		int iFrameEnd;
		float fChangeSec;
		float fTimeSum;
	} FRAME, Frame;

	/*D3D11_INPUT_ELEMENT_DESC
	 SemanticName : 성분에 부여된 문자열 이름. 이것은 정점 셰이더에서 시멘틱 이름으로 쓰이므로 반드시 유효한 변수 이름이어야 한다. 
					쉐이더 파일의 시멘틱 이름과 반드시 같아야함.

	 SemanticIndex: 시멘틱에 부여된 색인. 색인이 지정되지 않으면 0으로 간주

	 Format : DXGI_FORMAT 열거형의 한 멤버로，이 정점 성분의 자료 형식을 나타낸다. _float3인지 _float2인지 등에 따라 달라짐
	
	 InputSlot: 이 성분의 자료가 파이프라인에 공급될 정점 버퍼 슬롯의 색인이다.

	 AlignedByteOffset：입력 슬롯을 하나만 사용하는 경우 이 필드는 C++ 정점 구조체의 시작 위치와 이 정점 성분의 시작 위치 사이의 오프셋(바이트 단위)이다
	         
     InputSlotClass：일단 지금은 D3D11_INPUT_PER_VERTEX_DATA를 지정한다고 알아두고 넘어가자. 다른 값은고급 기법인 인스턴싱에 쓰인다.
	
	InstanceDataStepRate: 일단 지금은 0을 지정한다. 다른 값들은 고급 기법인 인스턴싱에 쓰인다
	*/


	const D3D11_INPUT_ELEMENT_DESC	VTXPOS::Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};


	const   D3D11_INPUT_ELEMENT_DESC	VTXPOSTEX::Elements[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const D3D11_INPUT_ELEMENT_DESC	VTXNORTEX::Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const D3D11_INPUT_ELEMENT_DESC	VTXMESH::Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const D3D11_INPUT_ELEMENT_DESC	VTXANIMMESH::Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};


}

