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
	/* ���������� ȣ�� : ������ �ʿ��� ����� ���ſ� �۾����� �����Ѵ�.(��Ŷ, ���� �����) */
	virtual HRESULT Initialize_Prototype();

	/* ��Ŷ�̳� ���� ������� ���ؼ� �޾ƿ��� ���ϴ� �����鵵 �и��� �����Ѵ�. */
	/* �������� �����ϴ� �ʴ� �߰����� �ʱ�ȭ�� �ʿ��� ��� ȣ���Ѥ���. */
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
		/*XMLoad- ���������� ���������*/
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

	const _uint* m_pParentState = { nullptr };  // �θ� ������ ����
	_uint m_iCurMotion = {}; // ���� �� ��� ����

	_bool m_bFinishAni = { false }; // �ִϸ��̼��� �������� �Ǵ�.

	RIM_LIGHT_DESC m_RimDesc{}; // �� ���� ����ü

	_float m_interver{ 0.f }; // ���̴��� �Ѱ� �� ������ ȿ�� �Ӱ谪
	_bool m_DyingTime = { false };
	_float m_fPlayAniTime = { 1.0 }; // �ִϸ��̼� ����ӵ�

	_float m_DyTimeSum = { 0.f };
	_float m_DyTime { 0.f };
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END