#pragma once

#include "Renderer.h"
#include "Sound.h"
#include "Component_Manager.h"
#include "GameObject.h"
#include "PipeLine.h"
#include "Calculator.h"
/* CGameInstance : */
/* 내 Engine에 유일하게 존재하는 싱글톤클래스다. */
/* Client사용자가 엔진의 기능을 이용하고자한다면 CGameInstance를 통해서 기능을 수행할 수 있도록 하겠다. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;
public:
	

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext);
	void	Update(_float fTimeDelta);
	void	Draw();
	void	Clear(_uint iClearLevelID);

public: /* For.Graphic_Device */
	HRESULT Render_Begin(_float4 Color);
	HRESULT Render_End();


public: /* For.Input_Device */
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
	_byte	Get_DIMouseDown(MOUSEKEYSTATE eMouse);
	_byte   Get_DIKeyDown(_ubyte byKeyID);

public: /* for.Timer_Manager */
	_float  Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void    Update_TimeDelta(const _wstring& strTimerTag);
#ifndef _DEBUG
	void    Get_FPS(const _wstring& pTimerTag, HWND g_hWnd);
#endif
public: /* for.Level_Manager */
	HRESULT Open_Level(_uint iCurrentLevelID, class CLevel* pNewLevel);


public: /* For.Object_Manager*/
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject_To_Layer(_uint iLevelIndex, const _uint& strLayerTag, const _wstring& strPrototypeTag, const _tchar* strProtoMapPath = nullptr, const _uint& DataType =0, void* pArg = nullptr);
	CGameObject* Clone_GameObject( const _wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Add_Clon_to_Layers(_uint iLevelIndex, const _uint& strLayerTag, class CGameObject* pGameObject);
     CGameObject* Find_Prototype(const _wstring& strPrototypeTag);


	/*---EDIT----*/
	map<const _wstring, class CGameObject*>		 Get_ProtoObject_map();
	_bool IsGameObject(_uint iLevelIndex, const _uint& strLayerTag);
	void ObjClear(_uint iLevelIndex);
	CGameObject::PICKEDOBJ_DESC Pking_onMash(_vector RayPos, _vector RayDir);
	CGameObject* Recent_GameObject(const _uint& strLayerTag);
	list<class CGameObject*> Get_ALL_GameObject(_uint iLevelIndex, const _uint& strLayerTag);

public: /* For.UI_Manager*/
	HRESULT Set_OpenUI(const _uint& uID, _bool UIopen);
	HRESULT Set_PreUpdateUI(const _uint& uID, _bool UIopen);
	HRESULT Set_UpdateUI(const _uint& uID, _bool UIopen);
	HRESULT Set_LateUpdateUI(const _uint& uID, _bool UIopen);
	CGameObject* Get_UI(const _uint& iLevel, const _uint& uID);

public: /* For.Component_Manager */
	HRESULT Add_Prototype_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	map<const _wstring, class CComponent*> Get_Com_proto_vec(_uint iLevelindex);

public: /* For.Sound*/
	void	Play_Sound(_tchar* pSoundKey,CSound::CHANNELID eID, _float fVolume);
	void	PlayBGM(_tchar* pSoundKey, _float fVolume);
	void	StopSound(CSound::CHANNELID eID);
	void	StopAll();
	void	SetChannelVolume(CSound::CHANNELID eID, _float fVolume);
	void	LoadSoundFile	(const _char* soundFile);

public: /* For.Renderer	*/
	HRESULT Add_RenderGameObject(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderGameObject);

public: /* For.PipeLine */
	const _float4x4* Get_TransformFloat4x4(CPipeLine::TRANSFORM_STATE eState);
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORM_STATE eState);
	const _float4* Get_CamPosition();
	void Set_TransformMatrix(CPipeLine::TRANSFORM_STATE eState, _fmatrix TransformMatrix);

public: /* For.Light_Manager */
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);

public: /* For.Calculator */
	_float3	Picking_OnTerrain(HWND hWnd, CVIBuffer_Terrain* pTerrainBufferCom, _matrix Proj, _matrix view, CTransform* Transform);
	void Make_Ray(HWND hWnd, _matrix Proj, _matrix view, _vector* RayPos, _vector* RayDir);
	_float Compute_Random_Normal();
	_float Compute_Random(_float fMin, _float fMax);


private:
	class CGraphic_Device*			m_pGraphic_Device	 = { nullptr };
	class CInput_Device*			m_pInput_Device		 = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager	 = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager	 = { nullptr }; 
	class CObject_Manager*			m_pObject_Manager	 = { nullptr };
	class CUI_Manager*				m_pUI_Manager		 = { nullptr };
	class CRenderer*				m_pRenderer			 = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CPipeLine*				m_pPipeLine			 = { nullptr };
	class CSound*					m_pSound			 = { nullptr };
	class CLight_Manager*			m_pLight_Manager	 = { nullptr };
	class CCalculator*              m_pCalculator		 = { nullptr };
public:
	static void  Release_Engine(); // 레퍼런스 카운트 누수를 막기위해 한 번 더 호출
	virtual void Free() override;
};

END