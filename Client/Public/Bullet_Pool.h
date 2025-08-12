#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Bullet.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)
class CBullet_Pool final : public CBase
{
public:


public:
    CBullet_Pool();
    virtual ~CBullet_Pool() = default;

public:
    CGameObject* GetBullet(void* Arg);

private:
    HRESULT Initialize_Prototype(size_t size, void* Arg);

private:
    class CGameInstance* m_pGameInstance = {nullptr};
    vector<class CGameObject*> m_vecBullet;

public:
    static CBullet_Pool* Create(size_t size, void* Arg);
    virtual void Free() override;
};
END