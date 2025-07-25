﻿#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

namespace Engine
{
	typedef		bool						_bool;

	typedef		signed char					_byte;
	typedef		unsigned char				_ubyte;
	typedef		char						_char;
	typedef		wchar_t						_tchar;
	typedef     wstring                     _wstring;

	typedef		signed short				_short;
	typedef		unsigned short				_ushort;

	typedef		signed int					_int;
    typedef     signed __int32              int32;
    typedef     signed __int64              int64;  
       

	typedef		unsigned int				_uint;
    typedef     unsigned __int16            _uint16;
    typedef     unsigned __int32            _uint32;
   


	typedef		signed long					_long;
	typedef		unsigned long				_ulong;

	typedef		float						_float;
	typedef		double						_double;

	/* 단순 저장용 벡터와 행렬 타입들을 의미한다. (대입연산만 가능함으로 연산용x) */
	//d3dx헤더가 포함되지 않았음으로 벡터 타입을 대체할 용도,
	typedef		XMFLOAT2					_float2;    
	typedef		XMFLOAT3					_float3;
	typedef		XMFLOAT4					_float4;

	typedef		XMFLOAT4X4					_float4x4;


	/* 저장이 가능하긴하지만 연산용 벡터와 행렬을 의미한다. (연산용)*/
	typedef		XMVECTOR					_vector;
	typedef		FXMVECTOR					_fvector;
	typedef		GXMVECTOR					_gvector;
	typedef		HXMVECTOR					_hvector;
	typedef		CXMVECTOR					_cvector;
	typedef		XMMATRIX					_matrix;
	typedef		FXMMATRIX					_fmatrix;
	typedef		CXMMATRIX					_cmatrix;
}

#endif // Engine_Typedef_h__
