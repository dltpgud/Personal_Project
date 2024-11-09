#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "UI_Manager.h"
#include "PipeLine.h"
#include "Input_Device.h"
#include "Light_Manager.h"
#include "Collider_Manager.h"
#include "Font_Manager.h"
#include "Target_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC & EngineDesc, _Out_ ID3D11Device ** ppDevice, _Out_ ID3D11DeviceContext ** ppContext)
{
	/* 내 게임에 필요한 필수 기능들에 대한 초기화과정을 수행한다. */
	/* 그래픽 카드를 초기화한다 */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowed, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	/* 입력 장치를 초기화한다. */
	m_pInput_Device = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	/* 매니져를 준비한다. */
	m_pTimer_Manager	 = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;
	
	m_pComponent_Manager = CComponent_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pLight_Manager =  CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pLevel_Manager     = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pObject_Manager    = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pUI_Manager		 = CUI_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	m_pSound		     = CSound::Create();
	if (nullptr == m_pSound)
		return E_FAIL;
	
	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer		     = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine			 = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pCalculator = CCalculator::Create(*ppDevice, *ppContext, EngineDesc.hWnd);
	if (nullptr == m_pCalculator)
		return E_FAIL;

	m_pCollider_Manager = Collider_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;


	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;



	return S_OK;
}

void CGameInstance::Update(_float fTimeDelta)
{

	/* 엔진에있는 객체들 중 반복적인 갱신이 필요한 녀석이라면 여기서 다 호출. */

    
	m_pInput_Device->Update_InputDev();



	m_pObject_Manager->Priority_Update(fTimeDelta);
	m_pUI_Manager->Priority_Update(fTimeDelta);
	m_pPipeLine->Update();


	
	m_pObject_Manager->Update(fTimeDelta);
	m_pUI_Manager->Update(fTimeDelta);


	m_pObject_Manager->Late_Update(fTimeDelta);
	m_pUI_Manager->Late_Update(fTimeDelta);
    m_pCollider_Manager->All_Collison_check();
	m_pLevel_Manager->Update(fTimeDelta);

}

void CGameInstance::Draw()
{
	/* 게임내에 필요한 대다수의 객체들을 모두 그려낸다. */
	m_pRenderer->Draw();

	/* 할일이 없어. 디버그모드에서만 디버그내용만 출력하는 용도 .*/
	m_pLevel_Manager->Render();
}

void CGameInstance::Clear(_uint iClearLevelID)
{
	/*iClearLevelID에 해당하는 자원들을 정리한다.*/	
	m_pUI_Manager->Clear(iClearLevelID);
	m_pObject_Manager->Clear(iClearLevelID);
	m_pComponent_Manager->Clear(iClearLevelID);
	m_pCollider_Manager->Clear(iClearLevelID);

}

void CGameInstance::Set_Player(CGameObject* pPlayer)
{
	m_pPlayer =  static_cast <CActor*>(pPlayer);

}

CActor* CGameInstance::Get_Player()
{
	return m_pPlayer;
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->Free();
	CGameInstance::DestroyInstance();
}

#pragma region Graphic_Device
HRESULT CGameInstance::Render_Begin(_float4 Color)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	/* 백버퍼를 내가 지정한 색으로 클리어(초기화)한다. */
	m_pGraphic_Device->Clear_BackBuffer_View(Color);
	/* 깊이버퍼와 스텐실버퍼를 내가 지정한 값으로 클리어(초기화)한다.*/
	m_pGraphic_Device->Clear_DepthStencil_View();
	return S_OK;
}

HRESULT CGameInstance::Render_End()
{

	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device-> Present();
	return S_OK;
}
#pragma endregion

#pragma region Input_Device
_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

_byte CGameInstance::Get_DIMouseDown(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseDown(eMouse);
}

_byte CGameInstance::Get_DIKeyDown(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyDown(byKeyID);
}

_byte CGameInstance::Get_DIAnyKey()
{
	return m_pInput_Device->Get_DIAnyKey();
}

void CGameInstance::MouseFix()
{
	m_pInput_Device->Mouse_Fix();
}

#pragma endregion

#pragma region Timer_Manager
_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;

	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Update_TimeDelta(const _wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	return m_pTimer_Manager->Update_TimeDelta(strTimerTag);
}
#ifndef _DEBUG
void CGameInstance::Get_FPS(const _wstring& pTimerTag, HWND g_hWnd)
{
	if (nullptr == m_pTimer_Manager)
		return ;

	return m_pTimer_Manager->Get_FPS(pTimerTag, g_hWnd);
}
#endif // !_DEBUG
#pragma endregion

#pragma region Level_Manager 
HRESULT CGameInstance::Open_Level(_uint iCurrentLevelID, CLevel* pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pLevel_Manager->Open_Level(iCurrentLevelID, pNewLevel);
	
	return S_OK;
}

_uint CGameInstance::Get_iCurrentLevel()
{
	if (nullptr == m_pLevel_Manager)
		return 0;
	return m_pLevel_Manager->Get_iCurrentLevel();
}

_bool CGameInstance::IsOpenStage()
{
	if (nullptr == m_pLevel_Manager)
		return false;
	return m_pLevel_Manager->IsOpenStage();
}

void CGameInstance::Set_Open_Bool(_bool NextStage)
{
	if (nullptr == m_pLevel_Manager)
		return;

	m_pLevel_Manager->Set_Open_Bool(NextStage);
}

#pragma endregion

#pragma region Object_Manager
HRESULT CGameInstance::Add_Prototype(const _wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}

map<const _wstring, class CGameObject*> CGameInstance::Get_ProtoObject_map()
{
	return m_pObject_Manager->Get_ProtoObject_map();
}

_bool CGameInstance::IsGameObject(_uint iLevelIndex, const _uint& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
	{
		MSG_BOX("Nullptr ObjMgr");
		return nullptr;
	}
	return m_pObject_Manager->IsGameObject(iLevelIndex,strLayerTag);
}
void CGameInstance::ObjClear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
	{
		MSG_BOX("Nullptr ObjMgr");
		return ;
	}

	m_pObject_Manager->Clear(iLevelIndex);
}

CGameObject::PICKEDOBJ_DESC CGameInstance::Pking_onMash(_vector RayPos, _vector RayDir)
{
	if (nullptr == m_pObject_Manager)
	{
		MSG_BOX("Nullptr ObjMgr");
	}

	return m_pObject_Manager->Pking_onMash(RayPos, RayDir);
}

CGameObject* CGameInstance::Recent_GameObject(_uint iLevelIndex, const _uint& strLayerTag)
{

	if (nullptr == m_pObject_Manager)
	{
		MSG_BOX("Nullptr ObjMgr");
		return nullptr;
	}

	return m_pObject_Manager->Recent_GameObject(iLevelIndex ,strLayerTag);
}

list<class CGameObject*> CGameInstance::Get_ALL_GameObject(_uint iLevelIndex, const _uint& strLayerTag)
{
	return m_pObject_Manager->Get_ALL_GameObject(iLevelIndex, strLayerTag);
}




HRESULT CGameInstance::Add_GameObject_To_Layer(_uint iLevelIndex, const _uint& strLayerTag, const _wstring& strPrototypeTag, const _tchar* strProtoMapPath , const _uint& DataType, void* pArg,  const _uint& ProtoTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	/*원본을 탐색한다*/
 	CGameObject* pPrototype = m_pObject_Manager->Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	if (nullptr != strProtoMapPath)
	{
		switch (DataType)
		{
		case CGameObject::DATA_TERRAIN : 
 					return m_pLevel_Manager->Load_to_Next_Map_terrain(iLevelIndex, strLayerTag, pPrototype, strProtoMapPath, pArg);
			break;

		case CGameObject::DATA_NONANIMAPOBJ:
		    	return m_pLevel_Manager->Load_to_Next_Map_NonaniObj(iLevelIndex, strLayerTag, pPrototype, strProtoMapPath, pArg);
			break;

		case CGameObject::DATA_WALL:
			    return m_pLevel_Manager->Load_to_Next_Map_Wall(iLevelIndex, strLayerTag, pPrototype, strProtoMapPath, pArg);
			break;

		case  CGameObject::DATA_MONSTER:
				return m_pLevel_Manager->Load_to_Next_Map_Monster(iLevelIndex, strLayerTag, pPrototype, ProtoTag, strProtoMapPath, pArg);
			break;

		case  CGameObject::DATA_NPC:
			    return m_pLevel_Manager->Load_to_Next_Map_NPC(iLevelIndex, strLayerTag, pPrototype, ProtoTag, strProtoMapPath, pArg);
			break;
		default:
			      return m_pLevel_Manager->Load_to_Next_Map_AniOBj(iLevelIndex, strLayerTag, pPrototype, DataType, strProtoMapPath, pArg);
			break;
		}
	
	}

	else /*또는 맵이 아니라면 바로 사본을 만들고 다음 단계 진행..*/
	{
		CGameObject* pGameObject = pPrototype->Clone(pArg);
		if (nullptr == pGameObject)
			return E_FAIL; 

		if (strLayerTag == CGameObject::UI)
			return m_pUI_Manager->Add_UI(pGameObject, iLevelIndex);
		else
			return m_pObject_Manager->Add_Clon_to_Layers(iLevelIndex, strLayerTag, pGameObject);
	}
	return S_OK;
}


HRESULT CGameInstance::Add_Clon_to_Layers(_uint iLevelIndex, const _uint& strLayerTag, CGameObject* pGameObject)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Clon_to_Layers(iLevelIndex, strLayerTag, pGameObject);
}

CGameObject* CGameInstance::Find_Prototype(const _wstring& strPrototypeTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Prototype(strPrototypeTag);
}
CGameObject* CGameInstance::Clone_Prototype(const _wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_Prototype(strPrototypeTag, pArg);
}
CGameObject* CGameInstance::Find_CloneGameObject(_uint iLevelIndex, const _uint& strLayerTag, const _uint& ProtoTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_CloneGameObject(iLevelIndex, strLayerTag, ProtoTag);
}
#pragma endregion


#pragma region Collider_Manager
HRESULT CGameInstance::Add_Monster(_uint iClearLevelID, CGameObject* Monster)
{
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;

	return m_pCollider_Manager->Add_Monster(iClearLevelID,Monster);
}

HRESULT CGameInstance::Add_MonsterBullet(_uint iClearLevelID, CGameObject* MonsterBUllet)
{
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;

	return m_pCollider_Manager ->Add_MonsterBullet(iClearLevelID, MonsterBUllet);
}

HRESULT CGameInstance::Player_To_Monster_Ray_Collison_Check()
{
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;

 	return m_pCollider_Manager->Set_Collison(true);
}

HRESULT CGameInstance::Find_Cell(_uint Ilevel)
{
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;

	return m_pCollider_Manager->Find_Cell(Ilevel);
}

#pragma endregion

#pragma region UI_Manager
HRESULT CGameInstance::Set_OpenUI(const _uint& uID, _bool UIopen)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Set_OpenUI(uID, UIopen);
}

HRESULT CGameInstance::Set_PreUpdateUI(const _uint& uID, _bool UIopen)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Set_PreUpdateUI(uID, UIopen);
}

HRESULT CGameInstance::Set_UpdateUI(const _uint& uID, _bool UIopen)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Set_UpdateUI(uID, UIopen);
}

HRESULT CGameInstance::Set_LateUpdateUI(const _uint& uID, _bool UIopen)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Set_LateUpdateUI(uID, UIopen);
}

CGameObject* CGameInstance::Get_UI(const _uint& iLevel, const _uint& uID)
{
    if (nullptr == m_pUI_Manager)
        return nullptr;
	return m_pUI_Manager->Get_UI(iLevel, uID);
}

HRESULT CGameInstance::UI_shaking(const _uint& uID, _float fTimeDelta)
{
    if (nullptr == m_pUI_Manager)
        return E_FAIL;


    return m_pUI_Manager->UI_shaking(uID, fTimeDelta);
}

HRESULT CGameInstance::Set_UI_shaking(const _uint& uID, _float fShakingTime, _float fPowerX, _float fPowerY)
{
    if (nullptr == m_pUI_Manager)
        return E_FAIL;

    return m_pUI_Manager->Set_UI_shaking(uID, fShakingTime, fPowerX, fPowerY);
}

HRESULT CGameInstance::Set_OpenUI_Inverse(const _uint& Openuid, const _uint& Cloaseduid)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;
	return m_pUI_Manager->Set_OpenUI_Inverse(Openuid,Cloaseduid);
}


#pragma endregion

#pragma region Component_Manager
HRESULT CGameInstance::Add_Prototype_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, CComponent* pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}
CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}
map<const _wstring, class CComponent*> CGameInstance::Get_Com_proto_vec(_uint iLevelindex)
{
	return m_pComponent_Manager->Get_Com_proto_vec(iLevelindex);
}
CComponent* CGameInstance::Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;
	return  m_pComponent_Manager->Find_Prototype(iLevelIndex, strPrototypeTag);
}
#pragma endregion

#pragma region Sound
void CGameInstance::Play_Sound(_tchar* pSoundKey, CSound::CHANNELID eID, _float fVolume)
{
	if (m_pSound == nullptr)
		return;
	return m_pSound->Play_Sound(pSoundKey, eID, fVolume);
}

void CGameInstance::PlayBGM(_tchar* pSoundKey, _float fVolume)
{
	if (m_pSound == nullptr)
		return;
	return m_pSound->PlayBGM(pSoundKey, fVolume);
}

void CGameInstance::StopSound(CSound::CHANNELID eID)
{
	if (m_pSound == nullptr)
		return;
	return m_pSound->StopSound(eID);
}

void CGameInstance::StopAll()
{
	if (m_pSound == nullptr)
		return;
	return m_pSound->StopAll();
}

void CGameInstance::SetChannelVolume(CSound::CHANNELID eID, _float fVolume)
{
	if (m_pSound == nullptr)
		return;
	return m_pSound->SetChannelVolume(eID, fVolume);
}

void CGameInstance::LoadSoundFile(const _char* soundFile)
{
	if (m_pSound == nullptr)
		return;
	return m_pSound->LoadSoundFile(soundFile);
}
#pragma endregion

#pragma region Renderer

HRESULT CGameInstance::Add_RenderGameObject(CRenderer::RENDERGROUP eRenderGroup, CGameObject* pRenderGameObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderGameObject(eRenderGroup, pRenderGameObject);
}
#pragma endregion

#pragma region PipeLine
const _float4x4* CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORM_STATE eState)
{
	if (nullptr == m_pPipeLine)
		return nullptr;

	return m_pPipeLine ->Get_TransformFloat4x4(eState);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORM_STATE eState)
{
	return m_pPipeLine->Get_TransformMatrix(eState);
}

const _float4* CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return nullptr;

	return m_pPipeLine->Get_CamPosition();
}

const _float4* CGameInstance::Get_CamLook()
{
	if (nullptr == m_pPipeLine)
		return nullptr;

	return m_pPipeLine->Get_CamLook();
}

void CGameInstance::Set_TransformMatrix(CPipeLine::TRANSFORM_STATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	return m_pPipeLine->Set_TransformMatrix(eState, TransformMatrix);
}

#pragma endregion


#pragma region Light_Manager
const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex)
{

	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(LightDesc);
}
#pragma endregion


#pragma region Calculator
_float3 CGameInstance::Picking_OnTerrain(HWND hWnd, CVIBuffer_Terrain* pTerrainBufferCom, _vector RayPos, _vector RayDir, CTransform* Transform, _float* fDis)
{
	if (nullptr == m_pCalculator)
		return _float3(0.f,0.f,0.f);

	return m_pCalculator->Picking_OnTerrain(hWnd, pTerrainBufferCom,  RayPos,  RayDir, Transform, fDis);
}

void CGameInstance::Make_Ray(_matrix Proj, _matrix view, _vector* RayPos, _vector* RayDir , _bool forPlayer)
{
	if (nullptr == m_pCalculator)
		return;

    m_pCalculator->Make_Ray(Proj, view, RayPos, RayDir, forPlayer);
	return;
}

_float CGameInstance::Compute_Random_Normal()
{
	if (nullptr == m_pCalculator)
		return _float(0.f);

	return m_pCalculator->Compute_Random_Normal();
}

_float CGameInstance::Compute_Random(_float fMin, _float fMax)
{
	if (nullptr == m_pCalculator)
		return _float(0.f);

	return m_pCalculator->Compute_Random(fMin, fMax);
}
HRESULT CGameInstance::Compute_Y(CNavigation* pNavigation, CTransform* Transform, _float3* Pos)
{
	if (nullptr == m_pCalculator)
		return E_FAIL;

	return m_pCalculator->Compute_Y(pNavigation, Transform, Pos);
}
_vector CGameInstance::PointNomal(_float3 fP1, _float3 fP2, _float3 fP3)
{
	return m_pCalculator->PointNomal(fP1, fP2, fP3);
}
#pragma endregion


#pragma region Font_Manager
HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, FXMVECTOR vColor, _float fScale, _float fRotation, const _float2& vPivot)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Text(strFontTag, pText, vPosition, vColor, fScale, fRotation, vPivot);
}
#pragma endregion

#pragma region Target_Manager
HRESULT CGameInstance::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return  m_pTarget_Manager->Add_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return  m_pTarget_Manager->Begin_MRT(strMRTTag);
}

HRESULT CGameInstance::End_MRT(const _wstring& strMRTTag)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->End_MRT(strMRTTag);
}

HRESULT CGameInstance::Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_RT_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}

#pragma endregion

void CGameInstance::Free()
{
	__super::Free();  // 소멸자가 디폴트임으로
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pCalculator);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pSound);
	Safe_Release(m_pComponent_Manager);	
	Safe_Release(m_pUI_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}
