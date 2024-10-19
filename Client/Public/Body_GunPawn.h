#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CBody_GunPawn : public CPartObject
{
public: 
	typedef struct BODY_GUNPAWN_DESC : CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
	}BODY_GUNPAWN_DESC;

private:
	CBody_GunPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_GunPawn(const CBody_GunPawn& Prototype);
	virtual ~CBody_GunPawn() = default;

public:
	/* ���������� ȣ�� : ������ �ʿ��� ����� ���ſ� �۾����� �����Ѵ�.(��Ŷ, ���� �����) */
	virtual HRESULT Initialize_Prototype() override;

	/* ��Ŷ�̳� ���� ������� ���ؼ� �޾ƿ��� ���ϴ� �����鵵 �и��� �����Ѵ�. */
	/* �������� �����ϴ� �ʴ� �߰����� �ʱ�ȭ�� �ʿ��� ��� ȣ���Ѥ���. */
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual CModel* Get_Model() override {
		return m_pModelCom;
	};
	_bool Get_Finish() { return m_bFinishAni; }



public:
	const _float4x4* Get_SocketMatrix(const _char* pBoneName);

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };



	const _uint* m_pParentState = { nullptr };
	_uint m_iCurMotion = {};
	_bool m_bFinishAni = { false };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_GunPawn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END