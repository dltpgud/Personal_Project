#pragma once

#include "Renderer.h"
#include "Sound.h"
#include "Component_Manager.h"
#include "GameObject.h"
#include "PipeLine.h"
#include "Calculator.h"
#include "Actor.h"
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


	void Set_Player(CGameObject* pPlayer);
    CActor* Get_Player();

public: /* For.Graphic_Device */
	HRESULT Render_Begin(_float4 Color);
	HRESULT Render_End();

public: /* For.Input_Device */
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
	_byte	Get_DIMouseDown(MOUSEKEYSTATE eMouse);
	_byte   Get_DIKeyDown(_ubyte byKeyID);
	_byte   Get_DIAnyKey();
	void	MouseFix();

public: /* for.Timer_Manager */
	_float  Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void    Update_TimeDelta(const _wstring& strTimerTag);
#ifndef _DEBUG
	void    Get_FPS(const _wstring& pTimerTag, HWND g_hWnd);
#endif
public: /* for.Level_Manager */
	HRESULT Open_Level(_uint iCurrentLevelID, class CLevel* pNewLevel);
	_uint Get_iCurrentLevel();
	_bool IsOpenStage();
	void Set_Open_Bool(_bool NextStage);

public: /* For.Object_Manager*/
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject_To_Layer(_uint iLevelIndex, const _uint& strLayerTag, const _wstring& strPrototypeTag, const _tchar* strProtoMapPath = nullptr, const _uint& DataType =0, void* pArg = nullptr, const _uint& ProtoTag = 0);
	HRESULT Add_Clon_to_Layers(_uint iLevelIndex, const _uint& strLayerTag, class CGameObject* pGameObject);
    CGameObject* Find_Prototype(const _wstring& strPrototypeTag);
    CGameObject* Clone_Prototype(const _wstring& strPrototypeTag, void* pArg = nullptr);
	CGameObject* Find_CloneGameObject(_uint iLevelIndex, const _uint& strLayerTag, const _uint& ProtoTag);
	/*---EDIT----*/
	map<const _wstring, class CGameObject*>		 Get_ProtoObject_map();
	_bool IsGameObject(_uint iLevelIndex, const _uint& strLayerTag);
	void ObjClear(_uint iLevelIndex);
	CGameObject::PICKEDOBJ_DESC Pking_onMash(_vector RayPos, _vector RayDir);
	CGameObject* Recent_GameObject(_uint iLevelIndex, const _uint& strLayerTag);
	list<class CGameObject*> Get_ALL_GameObject(_uint iLevelIndex, const _uint& strLayerTag);

public: /* For.Collider_Manager */
	HRESULT Add_Monster( class CGameObject* Monster);
	HRESULT Add_MonsterBullet( class CGameObject* MonsterBUllet);
	HRESULT Add_Collider(_float Damage, class CCollider* Collider);
	HRESULT Player_To_Monster_Ray_Collison_Check();
	HRESULT Find_Cell();


public: /* For.UI_Manager*/
	HRESULT Set_OpenUI(const _uint& uID, _bool UIopen);
	CGameObject* Get_UI(const _uint& iLevel, const _uint& uID);
    HRESULT UI_shaking(const _uint& uID, _float fTimeDelta);    
	HRESULT Set_UI_shaking(const _uint& uID, _float fShakingTime, _float fPowerX, _float fPowerY);
	HRESULT Set_OpenUI_Inverse(const _uint& Openuid, const _uint& Cloaseduid);



    public: /* For.Component_Manager */
	HRESULT Add_Prototype_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	map<const _wstring, class CComponent*> Get_Com_proto_vec(_uint iLevelindex);
	CComponent* Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);

public: /* For.Sound*/
	void	Play_Sound(_tchar* pSoundKey,CSound::CHANNELID eID, _float fVolume);
	void	PlayBGM(CSound::CHANNELID eID, _tchar* pSoundKey, _float fVolume);
	void	StopSound(CSound::CHANNELID eID);
	void	StopAll();
	void	SetChannelVolume(CSound::CHANNELID eID, _float fVolume);
	void	LoadSoundFile	(const _char* soundFile);
	

public: /* For.Renderer	*/
	HRESULT Add_RenderGameObject(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderGameObject);
	HRESULT Add_DebugComponents(class CComponent* pComponent);
public: /* For.PipeLine */
	const _float4x4* Get_TransformFloat4x4(CPipeLine::TRANSFORM_STATE eState);
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORM_STATE eState);
	const _float4x4* Get_ShadowTransformFloat4x4(CPipeLine::TRANSFORM_STATE eState);
	_matrix Get_ShadowTransformMatrix(CPipeLine::TRANSFORM_STATE eState);
	const _float4x4* Get_TransformFloat4x4_Inverse(CPipeLine::TRANSFORM_STATE eState);
	_matrix Get_TransformMatrix_Inverse(CPipeLine::TRANSFORM_STATE eState);
	const _float4* Get_CamPosition();
	const _float4* Get_CamLook();
	void Set_TransformMatrix(CPipeLine::TRANSFORM_STATE eState, _fmatrix TransformMatrix);
	void Set_ShadowTransformMatrix(CPipeLine::TRANSFORM_STATE eState, _fmatrix TransformMatrix);

public: /* For.Light_Manager */
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Light_Clear();

public: /* For.Calculator */
	_float3	Picking_OnTerrain(HWND hWnd, CVIBuffer_Terrain* pTerrainBufferCom, _vector RayPos, _vector RayDir, CTransform* Transform, _float* fDis);
    void Make_Ray(_matrix Proj, _matrix view, _vector* RayPos, _vector* RayDir ,_bool forPlayer = false);
	_float Compute_Random_Normal();
	_float Compute_Random(_float fMin, _float fMax);
	HRESULT Compute_Y(CNavigation* pNavigation, CTransform* Transform, _float3* Pos);
	_vector PointNomal(_float3 fP1, _float3 fP2, _float3 fP3);
	_bool IsPicked(_float3* pOut, _bool IsPlayer);


public: /* For.Font_Manager */
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, FXMVECTOR vColor, _float fScale = 1.f, _float fRotation = 0.f, const _float2& vPivot = _float2(0.f, 0.f));


public: /* For.Target_Manager */
		HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
		HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
		HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr, _bool isClear = true);
		HRESULT End_MRT(const _wstring& strMRTTag);
		HRESULT Bind_RT_SRV(class CShader* pShader, const _char* pConstantName, const _wstring& strTargetTag);
		HRESULT Copy_RT_Resource(const _wstring& strTargetTag, ID3D11Texture2D* pOut);
#ifdef _DEBUG
		HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
		HRESULT Render_RT_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif


#pragma region FRUSTUM
public: /* For.Frustum */
	_bool isIn_Frustum_WorldSpace(_fvector vTargetPos, _float fRange = 0.f);
    _bool isIn_Frustum_LocalSpace(_fvector vTargetPos, _float fRange = 0.f);
    void Frustum_Transform_To_LocalSpace(_fmatrix WorldMatrixInv);
#pragma endregion




private:
	 CActor*						m_pPlayer			 = { nullptr };   // 플레이어 포인터는 오브젝트 메니저가 지워질때 같이 지워줌으로 따로 지워줄 필요는 없다!.
	class Collider_Manager*			m_pCollider_Manager  = { nullptr };
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
	class CFont_Manager*			m_pFont_Manager		 = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager	 = { nullptr };
	class CFrustum*					m_pFrustum		     = { nullptr };
 public:
	static void  Release_Engine(); // 레퍼런스 카운트 누수를 막기위해 한 번 더 호출
	virtual void Free() override;
};

END