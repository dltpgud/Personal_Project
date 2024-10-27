#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CMonsterHP : public CPartObject
{
public:
	typedef struct CMonsterHP_DESC : public CPartObject::PARTOBJECT_DESC
	{
		_float fHP{};
		_float fMaxHP{};
	}CMonsterHP_DESC;

private:
	CMonsterHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterHP(const CMonsterHP& Prototype);
	virtual ~CMonsterHP() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Monster_HP( _float fHp) {
		m_fHP = fHp;
	}
	void Set_HitStart(_bool start) {
	 	m_bStart = start;
	}
	void Set_Hit(_bool bHit) {
	 	m_bHit = bHit;
	}
	void Set_bLateUpdaet(_bool bLateUpdate)
	{
		m_bLateUpdaet = bLateUpdate;
	}
private:
	CTexture*	    m_pTextureCom = {};
	CShader*		m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	_float m_fHP{};
	_float m_fMaxHP{};
	_float m_fHP_Pluse = { 0.f };
	_bool m_bStart = {false};
	_float m_fRatio = { 0.f };
	_bool m_bLateUpdaet = { false };
	_bool m_bHit = { false };
private:
	HRESULT Add_Components();
	

public:
	static CMonsterHP*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;
};
END
