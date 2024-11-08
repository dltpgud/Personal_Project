#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Terrain;
class CNavigation;
END

BEGIN(Tool)
class CTerrain : public CGameObject
{

private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& Prototype);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	
	virtual HRESULT Render() override;

	virtual void Set_Model(const _wstring& protoModel, _uint ILevel) override;
	virtual void Set_Buffer(_uint x, _uint y) override;

	virtual void Set_Scalra_uint(_uint scalra); 
	virtual void Set_Scalra_float(_float scalra);

 	CVIBuffer_Terrain* Get_buffer() { return m_pVIBufferCom; }
	virtual _float3* Get_VtxPos();
	virtual _wstring Get_ComPonentName() override { return m_wModel; }

	virtual _tchar* Get_ProtoName() override;

	virtual _uint Get_Scalra() override { return m_bFire; };
	virtual _float Get_Scalra_float() override { return m_iUVoffset; };

 _uint Get_SizeX()  {
		return m_pSize[0];
	}
_uint Get_SizeY()  {
		return m_pSize[1];
	}

private:
	CTexture*					 m_pTextureCom = { nullptr };
	CShader*					 m_pShaderCom = { nullptr };
	CVIBuffer_Terrain*			 m_pVIBufferCom = { nullptr };

	_uint    m_pSize[2]{};
	_wstring m_wModel;
	_tchar* m_Proto = {nullptr};
	_uint m_bFire{};
	_float m_iUVoffset{ 0.f};
	 _float* pFloat;
	_float m_fTimeSum{ 0.f };
private:
	HRESULT Add_Components();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;
};

END