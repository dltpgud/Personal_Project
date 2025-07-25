#pragma once

#include "Component.h"


BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };

protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

 public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg) override;
     _bool IsColl();

    public:
	_bool Intersect(CCollider* pTargetCollider);
	_bool RayIntersects(_vector RayPos, _vector RayDir, _float& fDis);
	_float Get_iCurRadius() ;
    _float3 Get_iCurCenter();
#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };  // 툴킷에서 제공해주는 기본 쉐이더. 패스는 하나만 존재
	ID3D11InputLayout* m_pInputLayout = { nullptr };

#endif

public:
	void Update(_fmatrix WorldMatrix);

private:
	TYPE				m_eColliderType = { TYPE_END };

	class CBounding* m_pBounding = { nullptr };
	_bool				m_isColl = { false };




public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END