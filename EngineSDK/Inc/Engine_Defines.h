
#pragma once
#pragma warning (disable: 4251)

// 4251 맵컨테이너 오류 잡아줌.
  
#include <d3d11.h>
#include <d3dcompiler.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <DirectXCollision.h>

#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"
#include "DirectXTK\Effects.h"     // 툴킷에서 기본제공 해주는 쉐이더. 하나의 패스를 사용
#include "Effects11/d3dx11effect.h"


#include <DirectXMath.h>

using namespace DirectX;


#include <map>
#include <vector>
#include <unordered_map>
#include <string>
#include <list>
#include <algorithm>
#include <set>
using namespace std;


#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"

using namespace Engine;

namespace Engine
{


	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	enum aiTextureType {
		aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_AMBIENT, aiTextureType_EMISSIVE,
		aiTextureType_HEIGHT, aiTextureType_NORMALS, aiTextureType_SHININESS, aiTextureType_OPACITY, aiTextureType_DISPLACEMENT, aiTextureType_LIGHTMAP,
		aiTextureType_REFLECTION, aiTextureType_BASE_COLOR, aiTextureType_NORMAL_CAMERA, aiTextureType_EMISSION_COLOR, aiTextureType_METALNESS,
		aiTextureType_DIFFUSE_ROUGHNESS, aiTextureType_AMBIENT_OCCLUSION, aiTextureType_UNKNOWN
	};

	#define AI_TEXTURE_TYPE_MAX  aiTextureType_UNKNOWN
}
#ifdef _DEBUG
#include <iostream>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

#ifdef _WIN64
#include "fmod.hpp"
#include "fmod_errors.h"
#pragma comment (lib, "fmod_vc.lib")
#pragma comment (lib, "fmodL_vc.lib")
using namespace FMOD;
#endif