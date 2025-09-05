#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CBody_HealthBot : public CPartObject
{
public: 
	typedef struct BODY_HEALTHBOT_DESC : CPartObject::PARTOBJECT_DESC
	{

	}BODY_HEALTHBOT_DESC;

private:
	 enum STATE
     {
        ST_Idle,
        ST_Interactive,
     };

private:
	CBody_HealthBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_HealthBot(const CBody_HealthBot& Prototype);
	virtual ~CBody_HealthBot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void    Update(_float fTimeDelta) override;
	virtual void    Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
        virtual HRESULT Render_Shadow() override;
            _bool   Get_Finish() { return m_bFinishAni;}

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
    HRESULT Init_CallBack();

private:
    _uint  m_iCurMotion{};
    _bool m_bFinishAni{};

public:
	static CBody_HealthBot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END