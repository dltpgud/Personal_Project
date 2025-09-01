#pragma once

#include "Component.h"


BEGIN(Engine)
class CActor;
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

 public:
    _bool   IsColl();
	_bool   Intersect(CCollider* pTargetCollider);
	_bool   RayIntersects(_vector RayPos, _vector RayDir, _float& fDis);
	_float  Get_iCurRadius() ;
    _float3 Get_iCurCenter();
    
   
    void SetTriggerCallback(function<void(CActor* other, _bool bColliding, _bool bPlayer)> cb)
    {
        m_Callback = std::move(cb);
    }

    void CollUpdate(CActor* target);
    void Set_Info(void* pArg);
    _bool IsInside(const _float3& pos);
#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };  // ��Ŷ���� �������ִ� �⺻ ���̴�. �н��� �ϳ��� ����
	ID3D11InputLayout* m_pInputLayout = { nullptr };

#endif

public:
	void Update(_fmatrix WorldMatrix);

private:
	TYPE				m_eColliderType = { TYPE_END };
    function<void(CActor* other, _bool bColliding, _bool bPlayer)> m_Callback;
	class CBounding* m_pBounding = { nullptr };
	_bool				m_isColl = { false };
  



public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END