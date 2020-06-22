#include "stdafx.h"
#include "Stage.h"
#include "Export_Function.h"
#include <fstream>
#include "StaticObject.h"
#include "RussianHat.h"
#include "Shield.h"
#include "Field.h"
//#include "SoundMgr.h"
CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
	m_uiStageIdx = 0;
}

CStage::~CStage(void)
{

}

HRESULT CStage::Ready_Scene(void)
{

	FAILED_CHECK_RETURN(Engine::CScene::Ready_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogic_Layer(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_UI_Layer(L"UI"), E_FAIL);
	//CSoundMgr::GetInstance()->PlayBGM(L"Adam Levine - Lost Stars Lyrics.mp3");
	//CSoundMgr::GetInstance()->SetVolume(CSoundMgr::BGM, 0.5f);
	//���̴��������߰� 
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	return S_OK;
}

HRESULT CStage::LateReady_Scene(void)
{
	FAILED_CHECK_RETURN(Engine::CScene::LateReady_Scene(), E_FAIL);

	//Engine::CCamera* pCamera = dynamic_cast<Engine::CCamera*>(Engine::Get_GameObject(L"GameLogic", L"DynamicCamera"));
	//NULL_CHECK_RETURN(pCamera, E_FAIL);
	//Engine::Get_Renderer()->Set_Perspective(*pCamera->Get_Projection());

	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	_int iEvent=Engine::CScene::Update_Scene(fTimeDelta);

	if (CKeyMgr::GetInstance()->KeyDown(KEY_R))
	{
		Engine::CScene*		pScene = CField::Create(m_pGraphicDev);
		if (pScene == nullptr)
			return -1;

		Engine::SetUp_Scene(pScene);
		return iEvent;
	}

	m_fTime += fTimeDelta;


	return iEvent;
}

_int CStage::LateUpdate_Scene(const _float & fTimeDelta)
{
	return NO_EVENT;
}

void CStage::Render_Scene(void)
{
	++m_dwRenderCnt;

	if (m_fTime >= 1.f)
	{
		wsprintf(m_szFPS, L"FPS : %d", m_dwRenderCnt);
		m_fTime = 0.f;
		m_dwRenderCnt = 0;
	}

	Engine::Render_Font(L"Font_Default", m_szFPS, &_vec2(10.f, 10.f), D3DXCOLOR(1.f, 0.f, 0.f,1.f));

}

CStage* CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage*	pInstance = new CStage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CStage::Free(void)
{
	Engine::CScene::Free();
}

HRESULT CStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	//pLayer->set_LayerName(pLayerTag);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	Engine::CGameObject*		pGameObject = nullptr;

	// SkyBox
	pGameObject = CSkyBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);


	// Player
	pGameObject = CPlayer::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player", pGameObject), E_FAIL);

	// //Sword
	pGameObject = CSword::Create(m_pGraphicDev, 0);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword", pGameObject), E_FAIL);

	if (LOAD_MODE == 5)
	{
		pGameObject = CRussianHat::Create(m_pGraphicDev,L"RussianHat",0);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(pGameObject->Get_InstName().c_str(), pGameObject), E_FAIL);

		////Shield 
		pGameObject = CShield::Create(m_pGraphicDev, 0);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Shield", pGameObject), E_FAIL);

	}


	m_ppGameObjectMap = &pLayer->Get_ObjectMap();

	if (LOAD_MODE != 0)
		m_uiStageIdx = LOAD_MODE;

	switch ((LOADMODE)m_uiStageIdx)
	{
	case LOAD_NOMAL:
		Load_Text(L"../../Resource/Data/Base.txt");
		break;
	case LOAD_NOMAL2:
		Load_Text(L"../../Resource/Data/SnowMap.txt");
		break;
	case LOAD_NOMAL3:
		Load_Text(L"../../Resource/Data/Base.txt");
		break;
	case LOAD_PLAYER:
		break;
	case LOAD_MONSTER:
		break;
	case LOAD_END:
		break;
	default:
		break;
	}
	


	m_pLayerMap.emplace(pLayerTag, pLayer);	

	return S_OK;
}

HRESULT CStage::Ready_UI_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;
/*
	auto	iter = find_if(m_pLayerMap.begin(), m_pLayerMap.end(), [](map<wstring, Engine::CLayer*>::const_iterator mapIter)
	{
		if (mapIter->first==(L"GameLogic"))
			return mapIter;
	}
	);

	if (iter == m_pLayerMap.end())
		return E_FAIL;

*/

//Engine::CTransform*	pPlayerTransform =	dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", Engine::ID_DYNAMIC));

	//NULL_CHECK_RETURN(pPlayerTransform, E_FAIL);

	// StaticCamera
	pGameObject = CThirdPersonCamera::Create(m_pGraphicDev,
		&_vec3(0.f, 10.f, -5.f),
		&_vec3(0.f, 0.f, 0.f),
		&_vec3(0.f, 1.f, 0.f),
		D3DXToRadian(45.f),
		_float(WINCX) / WINCY,
		1.f,
		1000.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"ThirdPersonCamera", pGameObject), E_FAIL);

	m_pLayerMap.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage::Ready_LightInfo(void)
{
	D3DLIGHT9			tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = D3DXCOLOR(0.95f, 0.95f, 1.0f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);
	tLightInfo.Direction = _vec3(1.f, -1.f, 1.f);

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0), E_FAIL);

	return S_OK;
}

//HRESULT CStage::Ready_Prototype(void)
//{
//	Engine::CComponent*		pComponent = nullptr;
//
//	// Optimization
//	pComponent = Engine::COptimization::Create(m_pGraphicDev, Engine::COptimization::Enable, 129, 129);
//	NULL_CHECK_RETURN(pComponent, E_FAIL);
//	Engine::Ready_Prototype(L"Proto_Optimization", pComponent);
//
//
//	return S_OK;
//}
//

HRESULT CStage::Load_Text(const _tchar * pFilePath)
{
	ifstream fin;

	fin.open(pFilePath);

	if (fin.fail())
		return E_FAIL;
	wstring wstrTemp;
	char cTemp[MIN_STR];
	Transform_Info tInfo;
	while (!fin.eof())
	{
		D3DXVECTOR3 vPos;

		fin.getline(cTemp, MIN_STR);
		wchar_t* ppwchar= CharToWChar(cTemp);
		wstrTemp = ppwchar;
		delete ppwchar;
		//wstring wstrConvert(cTemp, &cTemp[MIN_STR]);
		//wstrTemp = wstrConvert;

		fin.getline(cTemp, MIN_STR); // ������ ������ ���� ����(���� ����)�� �о����.
		tInfo.vScale.x = atof(cTemp);
		fin.getline(cTemp, MIN_STR); // ������ ������ ���� ����(���� ����)�� �о����.
		tInfo.vScale.y = atof(cTemp);
		fin.getline(cTemp, MIN_STR);
		tInfo.vScale.z = atof(cTemp);

		fin.getline(cTemp, MIN_STR); // ������ ������ ���� ����(���� ����)�� �о����.
		tInfo.vRotation.x = atof(cTemp);
		fin.getline(cTemp, MIN_STR); // ������ ������ ���� ����(���� ����)�� �о����.
		tInfo.vRotation.y = atof(cTemp);
		fin.getline(cTemp, MIN_STR);
		tInfo.vRotation.z = atof(cTemp);

		fin.getline(cTemp, MIN_STR); // ������ ������ ���� ����(���� ����)�� �о����.
		tInfo.vPosition.x = atof(cTemp);
		fin.getline(cTemp, MIN_STR); // ������ ������ ���� ����(���� ����)�� �о����.
		tInfo.vPosition.y = atof(cTemp);
		fin.getline(cTemp, MIN_STR);
		tInfo.vPosition.z = atof(cTemp);

		_uint uiNameCnt = wstrTemp.find_last_of(L'_');
		wstring wstrObjectName = wstrTemp.substr(0, uiNameCnt);

		_uint uiObjIdx = 0;
		uiObjIdx = _wtoi(wstrTemp.substr(uiNameCnt + 1, wstring::npos).c_str());

		//if (wstrTemp.find(L"SM_Ceilling_a_ba01") != wstring::npos)
		//	continue;
		//if (wstrTemp.find(L"AlterCeiling_ba01") != wstring::npos)
		//	continue;

		if (wstrTemp.compare(L"") == 0)
			break;




		Engine::CGameObject*		pGameObject = nullptr;
		pGameObject = CStaticObject::Create(m_pGraphicDev, wstrObjectName, uiObjIdx, tInfo);

		NULL_CHECK_RETURN(pGameObject, E_FAIL);

		(*m_ppGameObjectMap).insert(make_pair(wstrTemp, pGameObject));

	}
	fin.close();

	return S_OK;
}

wchar_t * CStage::CharToWChar(const char * pstrSrc)
{
	int nLen = strlen(pstrSrc) + 1;
	wchar_t* pwstr = (LPWSTR)malloc(sizeof(wchar_t)* nLen);
	mbstowcs(pwstr, pstrSrc, nLen);
	return pwstr;
}