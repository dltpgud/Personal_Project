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
	    _uint* pParentState = { nullptr };
	    _uint* pRimState = { nullptr };
	}PARTOBJECT_DESC;
protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& Prototype);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	const _float4x4* Get_SocketMatrix(const _char* pBoneName);

	_float Get_threshold() {return m_fthreshold;}
    void Set_DeadState(_bool DeadSt)
    {
            m_bDeadState = DeadSt;
    }

protected:
	const _float4x4*				m_pParentMatrix = { nullptr };
	_float4x4						m_WorldMatrix = {};   
	const _float4x4*				m_pSocketMatrix = { nullptr };
	
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	_uint* m_pParentState = { nullptr };  // 부모 파츠의 상태

	RIM_LIGHT_DESC m_RimDesc{}; // 림 연산 구조체

	_float m_fthreshold{ 0.f }; // 쉐이더에 넘겨 줄 디졸브 효과 임계값
	_bool m_bDeadState = { false };
	_float m_fDeadTimeSum = { 0.f };
	_float m_fDeadTime { 0.f };
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END