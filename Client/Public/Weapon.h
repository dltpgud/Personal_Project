#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon final : public CPartObject
{

public:
	enum WeaPoneType {
		HendGun, AssaultRifle, MissileGatling , HeavyCrossbow, WeaPoneType_END
	};
	enum WeaPoneState { Reload, Idle, Shoot };

	
public:
	typedef struct WEAPON_DESC : CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
		const _float4x4* pSocketMatrix = { nullptr };
		const _uint* pType = { nullptr };
	}WEAPON_DESC;
private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

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

	void Type0_Update(_float fTimeDelta);
	void Type2_Update(_float fTimeDelta);
	_float Damage() { return m_fDamage; }

	void Choose_Weapon(const _uint& WeaponNum);
	_uint  Get_Bullte() {
		return m_iMaxBullet[m_iBullet];
	}

	_uint  Get_MaxBullte() {
		return m_iFirstBullet[m_pWeapon];
	}
	HRESULT Make_Bullet(_float3 Offset);
	
private:

	CModel* m_pModelCom[WeaPoneType_END] = {nullptr};
	const _float4x4* m_pSocketMatrix = { nullptr };

	const _uint* m_pType = { nullptr };
	_float	m_fEmissivePower{ 0.f };
	_uint m_pWeapon{};
	_uint m_iMaxBullet[WeaPoneType_END]{0};
	_uint m_iFirstBullet[WeaPoneType_END]{ 0 };
	WeaPoneType	m_iBullet{};
	_float m_fDamage{};
	_bool m_istate{};
	_bool m_bEmissive = false;
	const _float4x4* m_pWeapon_SocketMatrix[WeaPoneType_END] = { nullptr };
	_float3 m_fWeaPonOffset{};
	class CShootingUI* m_ShootingUI = { nullptr};
	_float m_fTimeSum{ 0.f };
	_bool m_bClack = false;


	/*Sound*/
	_bool m_bHeandGunReLoad = false;
	_bool m_bAssaultRifleReLoad = false;
	_bool m_bHeavyCrossbowReload = false;
	_bool m_bMissileGatlingReload = false;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
