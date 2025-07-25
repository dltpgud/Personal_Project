#pragma once
#include "Client_Defines.h"
#include "UI.h"
#include "Player.h"
BEGIN(Engine)
	class CShader;
	class CTexture;
	class CVIBuffer_Rect;
	END

BEGIN(Client)

class CPlayerEffectUI: public CUI
{

		enum TYPE{ IDlE,SPRINT, HIT, HEALTH, TYPE_END};
public:
		typedef struct CPlayerEffectUI_DESC : public CUI::CUI_DESC
		{

			const _uint* State{};
		}CPlayerEffectUI_DESC;

private:
	CPlayerEffectUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerEffectUI(const CPlayerEffectUI& Prototype);
	virtual ~CPlayerEffectUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
        virtual void Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_scale(_int i);
public:
	const _uint* m_iPlayerState = nullptr;
	TYPE m_iType{};
	_uint m_iStateTex{ 0 };
	_uint m_iPass{};
	_int m_iDisCard{ -1 };
	_bool m_bDisCard = false;
	_float m_ScaleX{ 0.f };
	_float m_ScaleY{ 0.f };
	_float m_alpha[3];
	_int m_idiscardNum{};
	class CActor* m_pPlayer = { nullptr };
private:
	CTexture* m_pTextureCom[CPlayer::STATE_END] = {nullptr};
	CShader* m_pShaderCom= {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
private:
	HRESULT Add_Components();
	


public:	
	static CPlayerEffectUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;

};

END