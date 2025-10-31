#pragma once

#include "Renderer.h"
#include "Collider_Manager.h"
#include "Sound.h"
#include "Component_Manager.h"
#include "GameObject.h"
#include "PipeLine.h"
#include "Calculator.h"
#include "Actor.h"
#include "ThreadPool.h"
#include "Frustum.h"
#include "Effect_Manager.h"
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
    void    Delete();
	void	Clear(_uint iClearLevelID);

#pragma region Graphic_Device
public: /* For.Graphic_Device */
	HRESULT Render_Begin(_float4 Color);
	HRESULT Render_End();
#pragma endregion

#pragma region Input_Device
public: /* For.Input_Device */
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
	_byte	Get_DIMouseDown(MOUSEKEYSTATE eMouse);
    _byte   Get_DIMouseUp(MOUSEKEYSTATE eMouse);
	_byte   Get_DIKeyDown(_ubyte byKeyID);
	_byte   Get_DIAnyKey();
    _bool   MouseFix(_bool Fix= false);

#pragma endregion

#pragma region Timer_Manager
public: /* for.Timer_Manager */
	_float  Get_TimeDelta(const _wstring& strTimerTag);
    _float* Get_TimeDeltaSum(const _wstring& strTimerTag);
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void    Update_TimeDelta(const _wstring& strTimerTag);
#pragma endregion

#pragma region Level_Manager
public: /* for.Level_Manager */
	HRESULT Open_Level(_uint iCurrentLevelID, class CLevel* pNewLevel);
	_uint Get_iCurrentLevel();
	_bool IsOpenStage();
	void Set_Open_Bool(_bool NextStage);
#pragma endregion

#pragma region Object_Manager
public: /* For.Object_Manager*/
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
    HRESULT Add_GameObject_To_Layer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag,
                                    void* pArg = nullptr);
    HRESULT Add_Clon_to_Layers(_uint iLevelIndex, const _wstring& strLayerTag, class CGameObject* pGameObject);
    void Set_Player(const _wstring& ProtoTag, void* pArg = nullptr);
    CActor* Get_Player();
    CGameObject* Find_Prototype(const _wstring& strPrototypeTag);
    CGameObject* Clone_Prototype(const _wstring& strPrototypeTag, void* pArg = nullptr);
    CGameObject* Find_CloneGameObject(_uint iLevelIndex, const _wstring& strLayerTag, const _uint& ProtoTag);
	/*---EDIT----*/
	map<const _wstring, class CGameObject*>		 Get_ProtoObject_map();
    _bool IsGameObject(_uint iLevelIndex, const _wstring& strLayerTag );
	void ObjClear(_uint iLevelIndex);
    CGameObject::PICKEDOBJ_DESC Pking_onMash(const _uint& iLevelIndex, const _wstring& strLayerTag, _vector RayPos,
                                             _vector RayDir);
    CGameObject* Recent_GameObject(_uint iLevelIndex, const _wstring& strLayerTag);
    list<class CGameObject*> Get_ALL_GameObject(_uint iLevelIndex, const _wstring& strLayerTag);
    void Preallocate_GameObject(_wstring ProtoTag, size_t count, void* desc);
#pragma endregion

#pragma region Collider_Manager
    public: /* For.Collider_Manager */
    HRESULT Add_Collider(class CCollider* Collider, _int Damage = 0);
    HRESULT Add_GameObject_To_ColGroup(class CGameObject* Obj, const _uint& Type);
	HRESULT Player_To_Monster_Ray_Collison_Check();
	HRESULT Find_Cell();
   
#pragma endregion

#pragma region UI_Manager
public: /* For.UI_Manager*/
    HRESULT UI_shaking(const _uint& uID, _float fTimeDelta);    
	HRESULT Set_UI_shaking(const _uint& uID, _float fShakingTime, _float fPowerX, _float fPowerY);
    HRESULT ADD_UI_ShakingList(class CUI* UIOBJ);
    HRESULT Set_OpenUI(_bool bOpen, const _wstring& strCloneTag, class CGameObject* Owner = nullptr);
    CGameObject* Find_Clone_UIObj(const _wstring& strCloneTag);
    HRESULT Add_UI_To_Proto(const _wstring& strProtoTag, class CGameObject* pUI);
    HRESULT Add_UI_To_CLone(const _wstring& strCloneTag, const _wstring& strProtoTag, void* pArg = nullptr);
#pragma endregion

#pragma region Component_Manager
public: /* For.Component_Manager */
	HRESULT Add_Prototype_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	map<const _wstring, class CComponent*> Get_Com_proto_vec(_uint iLevelindex);
	CComponent* Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);
#pragma endregion

#pragma region Sound
public: /* For.Sound*/
    void    Play_Sound(_tchar* pSoundKey, FMOD::Channel** ppChannel, _float fVolume, _bool bLoop = false);
    void    PlayBGM(FMOD::Channel** ppChannel, _tchar* pSoundKey, _float fVolume);
	void	StopAll();
    void    UpdateSoundPosition(FMOD::Channel* pChannel, CTransform* pTransform);
    void    LoadSoundFile(const _char* soundFile, _bool isBGM = false);
#pragma endregion

#pragma region Renderer
public: /* For.Renderer	*/
	HRESULT Add_RenderGameObject(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderGameObject);
	HRESULT Add_DebugComponents(class CComponent* pComponent);
#pragma endregion

#pragma region PipeLine
public: /* For.PipeLine */
	const _float4x4* Get_TransformFloat4x4(CPipeLine::TRANSFORM_STATE eState);
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORM_STATE eState);
	const _float4x4* Get_ShadowTransformFloat4x4(CPipeLine::TRANSFORM_STATE eState);
	_matrix Get_ShadowTransformMatrix(CPipeLine::TRANSFORM_STATE eState);
	const _float4x4* Get_TransformFloat4x4_Inverse(CPipeLine::TRANSFORM_STATE eState);
    const _float4x4* Get_ShadowTransformFloat4x4_Inverse(CPipeLine::TRANSFORM_STATE eState);
  
	_matrix Get_TransformMatrix_Inverse(CPipeLine::TRANSFORM_STATE eState);
	const _float4* Get_CamPosition();
	const _float4* Get_CamLook();
    const _float4* Get_CamUp();
    const _float* Get_CamNear();
    void Set_Camfar(_float fFar);
    void Set_CamNear(_float fNear);
    const float* Get_CamFar();
	void Set_TransformMatrix(CPipeLine::TRANSFORM_STATE eState, _fmatrix TransformMatrix);
	void Set_ShadowTransformMatrix(CPipeLine::TRANSFORM_STATE eState, _fmatrix TransformMatrix);  

#pragma endregion

#pragma region Light_Manager 
public: /* For.Light_Manager */
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Light_Clear();
#pragma endregion

#pragma region Calculator
public: /* For.Calculator */
     _float3 Picking_OnTerrain( CVIBuffer_Terrain* pTerrainBufferCom, _vector RayPos, _vector RayDir,
                                  CTransform* Transform, _float* fDis, _float3* vNormal = nullptr);
    void Make_Ray(_matrix Proj, _matrix view, _vector* RayPos, _vector* RayDir ,_bool forPlayer = false);
	_float Compute_Random_Normal();
	_float Compute_Random(_float fMin, _float fMax);
	_vector PointNomal(_float3 fP1, _float3 fP2, _float3 fP3);
    _bool RayIntersectsAABB_Local(_vector rayO_L, _vector rayD_L, const _float3& min, const _float3& max);
     
#pragma endregion

#pragma region Font_Manager
public: /* For.Font_Manager */
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, FXMVECTOR vColor, _float fScale = 1.f, _float fRotation = 0.f, const _float2& vPivot = _float2(0.f, 0.f));
#pragma endregion

#pragma region Target_Manager
public: /* For.Target_Manager */
		HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
		HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
		HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr, _bool isClear = true);
		HRESULT End_MRT(const _wstring& strMRTTag);
		HRESULT Bind_RT_SRV(class CShader* pShader, const _char* pConstantName, const _wstring& strTargetTag);
		HRESULT Copy_RT_Resource(const _wstring& strTargetTag, ID3D11Texture2D* pOut);
        ID3D11ShaderResourceView* Get_SRV(const _wstring& strTargetTag);

#ifdef _DEBUG
		HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
		HRESULT Render_RT_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
#pragma endregion

#pragma region FRUSTUM
public: /* For.Frustum */
	_bool isIn_Frustum_WorldSpace(_fvector vTargetPos, _float fRange = 0.f);
    _bool isIn_Frustum_LocalSpace(_fvector vTargetPos, _float fRange = 0.f);
    void Frustum_Transform_To_LocalSpace(_fmatrix WorldMatrixInv);
   void CalculateCascadeFrustum(const float* cascadeSplits, int numCascades);
#pragma endregion


#pragma region ThreadPool
public: /* For.ThreadPool */
    template <class T, class... Args>
    auto Add_Job(T&& f, Args&&... args) -> future<typename result_of<T(Args...)>::type>;
    _bool AllJobCompleted();
#pragma endregion

#pragma region Decal
    HRESULT Add_DecalProto(const wstring& Key, const _tchar* FilePath, const _uint& TexNum =1);
    HRESULT Add_Decal(const wstring& Key, const DECAL_DESC* DecalDesc, _float fTimeDelta = 0.f);
    HRESULT Render_Decal(class CShader* pShader);
    HRESULT Decal_Clear();
    class CDecal* Find_Prototype_Decal(const _wstring& strPrototypeTag);
    HRESULT BuildGlobalDecalArray();
    void Preallocate_Decal(_wstring ProtoTag, size_t count, void* desc);
#pragma endregion


	 HRESULT Render_AllDecal(class CShader* pShader = nullptr);  
    HRESULT Render_All(class CShader* pShader = nullptr);

    // 특정 스트림 등록/조회
    HRESULT Add_EffectStream(const _wstring& key, class CEffectStream* pStream);

    // 이펙트 트리거
    HRESULT Trigger_Effect(const _wstring& streamKey, void* pSpawnDesc, _float fTimeDelta = 0);
    CEffectStream* Find_EffectStream(const _wstring& key);

private:
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
    class CThreadPool*              m_pThreadPool        = { nullptr };
    class CDecal_Manager*           m_pDecal_Manager     = { nullptr }; 
	class CEffect_Manager*          m_pEffect_Manager    = { nullptr }; 
 public:
	static void  Release_Engine(); // 레퍼런스 카운트 누수를 막기위해 한 번 더 호출
	virtual void Free() override;
};

END

template <class T, class... Args>
    inline auto CGameInstance::Add_Job(T&& f, Args&&... args) -> future<typename result_of<T(Args...)>::type>
{
    return CGameInstance::m_pThreadPool->Add_Job(f, args...);
}

	
	