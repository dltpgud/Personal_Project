#pragma once

#include "GameObject.h"
#include "Component_Manager.h"
BEGIN(Engine)
class CShader;
class CModel;
class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct PARTOBJECT_DESC : CGameObject::GAMEOBJ_DESC
	{
		const _float4x4*	pParentMatrix = { nullptr };
		const _uint* pParentState = { nullptr };
		const _uint* pRimState = { nullptr };
	}PARTOBJECT_DESC;
protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& Prototype);
	virtual ~CPartObject() = default;

public:
	/* 원형생성시 호출 : 생성시 필요한 상당히 무거운 작업들을 수행한다.(패킷, 파일 입출력) */
	virtual HRESULT Initialize_Prototype();

	/* 패킷이나 파일 입출력을 통해서 받아오지 못하는 정보들도 분명히 존재한다. */
	/* 원형에게 존재하는 않는 추가적인 초기화가 필요한 경우 호출한ㄴ다. */
	virtual HRESULT Initialize(void* pArg);
	virtual _int Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual CModel* Get_Model() {
		return m_pModelCom;
	};
	_bool Get_Finish() { return m_bFinishAni; }

	const _float4x4* Get_SocketMatrix(const _char* pBoneName);

	void Set_BoneUpdateMatrix(const _char* pBoneName, _fmatrix NewMatrix);

	void Set_CurMotion(_uint Motion) {
		m_iCurMotion = Motion;
	}
	_vector Get_PartObj_Position()
	{
		/*XMLoad- 저장용행렬을 연산용으로*/
		return XMLoadFloat4x4(&m_WorldMatrix).r[3];
	};

	_float Get_interver() {return m_interver;}

protected:
	const _float4x4*				m_pParentMatrix = { nullptr };
	_float4x4						m_WorldMatrix = {};   
	const _float4x4*				m_pFindBonMatrix = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	const _uint* m_pParentState = { nullptr };  // 부모 파츠의 상태
	_uint m_iCurMotion = {}; // 현재 내 모션 상태

	_bool m_bFinishAni = { false }; // 애니메이션이 끝났는지 판단.

	RIM_LIGHT_DESC m_RimDesc{}; // 림 연산 구조체

	_float m_interver{ 0.f }; // 쉐이더에 넘겨 줄 디졸브 효과 임계값
	_bool m_DyingTime = { false };
	_float m_fPlayAniTime = { 1.0 }; // 애니메이션 재생속도

	_float m_DyTimeSum = { 0.f };
	_float m_DyTime { 0.f };
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END