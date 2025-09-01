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

class CPlayer_StateUI: public CUI
{
    enum TYPE
    {
        SPRINT,
        DEFULT,
		COLOR,
        TYPE_END
    };

public:
	typedef struct CPlayerEffectUI_DESC : public CUI::CUI_DESC
	{
            const _uint* State{};
	}CPlayerEffectUI_DESC;

private:
	CPlayer_StateUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_StateUI(const CPlayer_StateUI& Prototype);
	virtual ~CPlayer_StateUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
    virtual void    ShakingEvent(_float fTimeDelta) override;
    void            Set_scale(_int i);

public:
	TYPE m_iType{};
	_uint m_iStateTex{ 0 };
	_uint m_iPass{};
	_bool m_bDisCard = false;
	_float m_ScaleX{ 0.f };
	_float m_ScaleY{ 0.f };
	_float3 m_alpha{};
    _float3 m_fStateColor{};
     const _uint* m_pPlayerState{};

private:
	CTexture* m_pTextureCom[5] = {nullptr};
	CShader* m_pShaderCom= {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	HRESULT Add_Components();

public:	
	static CPlayer_StateUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;

};

END