#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CWeapon final : public CPartObject
{
public:
	enum WeaPoneType {
		HendGun, AssaultRifle, MissileGatling , HeavyCrossbow, WeaPoneType_END
	};
	enum WeaPoneState { WS_RELOAD, WS_IDLE, WS_SHOOT };

public:
	typedef struct WEAPON_DESC : CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr };
	}WEAPON_DESC;

	struct WEAPON_NODE_DESC
    {
       CModel*           pModelCom{nullptr};    
       _uint             iBodyType{};
       _float3           iBulletOffset{};
       const _float4x4*  pBoneMatrix = {nullptr};
       _int              iMaxBullet{};
       _int              iCurBullet{};
       _float            fEmissvePower{};
       _float            fPreEmissvePower{};
       _float            fFireRate{};
       _int              Damage{};
       _float4           BulletColor[2]{};
       _float2           BulletScale{};
    };

private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void    Choose_Weapon(const _uint& WeaponNum);
    _uint*  Get_Weapon(){return &m_iWeapon;}
	WEAPON_NODE_DESC Get_Weapon_Info() const {return m_vecWeaPone[m_iWeapon];}
    void    Weapon_CallBack(_int WeaPonType, _uint AnimIdx, _int Duration, function<void()> func);
    HRESULT Set_Animation(_int Index, _bool IsLoop);
    _bool   Play_Animation(_float fTimeDelta);

private:
	const _float4x4*         m_pSocketMatrix = { nullptr };
	_uint                    m_iWeapon{};
	vector<WEAPON_NODE_DESC> m_vecWeaPone;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
    void    ResetEmissive(){m_vecWeaPone[m_iWeapon].fEmissvePower = m_vecWeaPone[m_iWeapon].fPreEmissvePower;}
    void    ResetBullet()  {m_vecWeaPone[m_iWeapon].iCurBullet = m_vecWeaPone[m_iWeapon].iMaxBullet;}
    HRESULT Make_Bullet();
    HRESULT Init_Weapon();
    HRESULT Init_CallBack();

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
