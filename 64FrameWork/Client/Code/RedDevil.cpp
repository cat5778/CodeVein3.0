#include "stdafx.h"
#include "RedDevil.h"
#include "Export_Function.h"
#include "ColliderManager.h"
#include "Shield.h"

CRedDevil::CRedDevil(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstrName,_uint uiIdx, _uint uiStageIdx )
	: CDynamicObject(pGraphicDev,wstrName,uiIdx, uiStageIdx)
{
	m_bIsMonster = true;

}

CRedDevil::CRedDevil(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstrName, _uint uiIdx, TRANSFORM_INFO tInfo, _uint uiStageIdx)
	:CDynamicObject(pGraphicDev,wstrName,uiIdx,tInfo, uiStageIdx)

{
	m_bIsMonster = true;
}

CRedDevil::~CRedDevil()
{
}

HRESULT CRedDevil::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fCurHp = m_fMaxHp =30.f;
	m_fAttackRange = 4.f;
	Set_TransformData();

	m_eCurState = RED_NoneFight_Idle;
	m_ePreState = RED_RedDevil_End;

	if (LOAD_MODE != 0)
		m_uiStageIdx = LOAD_MODE;
	switch ((LOADMODE)m_uiStageIdx)
	{
	case LOAD_NOMAL:
		m_pNaviCom->Set_Index(38);// Base Init Idx 38 
		Load_Text(L"../../Resource/Data/NavMash/BaseCompleteNav.txt");
		break;
	case LOAD_NOMAL2:
		m_pTransformCom->Set_Pos(-20.2f, 1.9f, -49.f);
		m_pNaviCom->Set_Index(111);// Base Init Idx 38 
		Load_Text(L"../../Resource/Data/NavMash/Temp5.txt");
		break;
	case LOAD_NOMAL3:
		break;

	case LOAD_PLAYER:
		break;
	case LOAD_MONSTER:
		break;
	case LOAD_BATTLE:
		//눈맵 플레이어바로앞
		m_pTransformCom->Set_Pos(-12.32f, 2.73f, -25.3241f);
		m_pNaviCom->Set_Index(76);// Base Init Idx 38 
		Load_Text(L"../../Resource/Data/NavMash/Temp5.txt");
		break;


	case LOAD_END:
		break;
	default:
		break;
	}



	return S_OK;
}

HRESULT CRedDevil::LateReady_GameObject()
{
	m_pTargetTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", Engine::ID_DYNAMIC));

	if (m_ppGameObjectMap == nullptr)
		m_ppGameObjectMap = &Engine::Get_Layer(L"GameLogic")->Get_ObjectMap();


	return S_OK;
}

_int CRedDevil::Update_GameObject(const _float & fTimeDelta)
{


	//if (CKeyMgr::GetInstance()->KeyDown(KEY_NUM1))
	//{
	//	m_uiAni++;
	//	cout << m_uiAni << endl;
	//}
		//StateMachine();
		//Pattern(fTimeDelta);
	//m_pColliderGroupCom->Set_ColliderEnable(Engine::COLOPT_ATTACK, true);
	//cout << "근접 몬스터 체력= " << m_fCurHp << endl;
	//cout << m_eCurState << endl;
	//cout << "현재거리 " << m_fDistance << endl;
	m_pColliderGroupCom->Set_ColliderEnable(Engine::COLOPT_ATTACK, false);


	srand((unsigned int)time(NULL));
	m_fDistance = Get_TargetDist();
	KnockBack(fTimeDelta);
	StateMachine();

	if (!(m_eCurState >= RED_Down_S&& m_eCurState <= RED_Damage_BL))
		Pattern(fTimeDelta);
	else
	{
		HurtMoveSet(fTimeDelta);
	}

	Collision_Check(fTimeDelta);


	Engine::CGameObject::Update_GameObject(fTimeDelta);
	if(!m_bIsMonDead)
		m_pMeshCom->Play_Animation(m_fAnimSpeed*fTimeDelta);
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return S_OK;
}

void CRedDevil::Render_GameObject(void)
{

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	pEffect->AddRef();

	_uint	iPassMax = 0;

	SetUp_ConstantTable(pEffect);

	pEffect->Begin(&iPassMax, 0);

	pEffect->BeginPass(0);

	m_pMeshCom->Render_Meshes(pEffect);

	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);

	m_pColliderGroupCom->Render_Collider();



}

HRESULT CRedDevil::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	pComponent = m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(RESOURCE_STAGE, L"RedDevil"));
	//pComponent = m_pMeshCom =Engine::CDynamicMesh::Create(m_pGraphicDev, L"../../Resource/Mesh/DynamicMesh/RedDevil/",L"RedDevil.X");
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_pComponentMap[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);// 변경

	pComponent = m_pTransformCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_pComponentMap[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_pComponentMap[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	pComponent = m_pCalculatorCom = Engine::CCalculator::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_pComponentMap[Engine::ID_STATIC].emplace(L"Com_Calculator", pComponent);

	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_pComponentMap[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	pComponent = m_pColliderGroupCom = Engine::CColliderGroup::Create(m_pGraphicDev, m_wstrInstName, m_pTransformCom, m_pMeshCom);
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_pComponentMap[Engine::ID_DYNAMIC].emplace(L"Com_ColliderGroup", pComponent);
	for (int i = 0; i < Engine::COLOPT_END; i++)
		m_pColliderGroupCom->Set_ColliderEnable((Engine::COLLOPTION)i, true);

	pComponent = m_pNaviCom = dynamic_cast<Engine::CNaviMesh*>(Engine::CNaviMesh::Create(m_pGraphicDev));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_pComponentMap[Engine::ID_STATIC].emplace(L"Com_Navi", pComponent);

	CColliderManager::GetInstance()->Get_ObjCollider(m_pColliderGroupCom, m_ObjName);

	return S_OK;
}

void CRedDevil::Set_TransformData()
{
	if (m_bIsRespawn)
	{
		m_pTransformCom->m_vScale = m_tTransformInfo.vScale;
		m_pTransformCom->m_vAngle = m_tTransformInfo.vRotation;
		m_pTransformCom->m_vInfo[Engine::INFO_POS] = m_tTransformInfo.vPosition;
	}
	else
	{
		m_pTransformCom->Set_Scale(DEFAULT_MESH_SCALE);
		m_pTransformCom->Set_Pos(INIT_VEC3);
	}
}

void CRedDevil::StateMachine()
{
	if (m_ePreState != m_eCurState)
	{
		m_ePreState = m_eCurState;
		m_fAnimSpeed = 1.0f;

		switch (m_eCurState)
		{
		case RED_Death_F:
			m_pMeshCom->Set_AnimationSet(31);
			m_uiAni = 31;
			break;
		case RED_Death_B:
			m_uiAni = 32;
			m_pMeshCom->Set_AnimationSet(32);
			break;
		case RED_DownEnd_S:
			m_uiAni = 33;
			m_pMeshCom->Set_AnimationSet(33);
			break;
		case RED_DownEnd_P:
			m_uiAni = 34;
			m_pMeshCom->Set_AnimationSet(34);
			break;
		case RED_Down_S:
			m_uiAni = 35;
			m_pMeshCom->Set_AnimationSet(35);
			break;
		case RED_Down_P:
			m_uiAni = 36;
			m_pMeshCom->Set_AnimationSet(36);
			break;
		case RED_Damage_FR:
			m_uiAni = 37;
			m_pMeshCom->Set_AnimationSet(37);
			break;
		case RED_Damage_FL:
			m_uiAni = 38;
			m_pMeshCom->Set_AnimationSet(38);
			break;
		case RED_Damage_BR:
			m_uiAni = 39;
			m_pMeshCom->Set_AnimationSet(39);
			break;
		case RED_Damage_BL:
			m_uiAni = 40;
			m_pMeshCom->Set_AnimationSet(40);
			break;
		case RED_Run_F:
			m_uiAni = 41;
			m_pMeshCom->Set_AnimationSet(41);
			break;
		case RED_DashAttack_E:
			m_uiAni = 42;
			m_pMeshCom->Set_AnimationSet(42);
			break;
		case RED_DashAttack_L:
			m_uiAni = 43;
			m_pMeshCom->Set_AnimationSet(43);
			break;
		case RED_DashAttack_S:
			m_uiAni = 44;
			m_pMeshCom->Set_AnimationSet(44);
			break;
		case RED_Attack3:
			m_fAnimSpeed = 2.5f;
			m_uiAni = 45;
			m_pMeshCom->Set_AnimationSet(45);
			break;
		case RED_Attack2:
			m_fAnimSpeed = 2.5f;
			m_uiAni = 46;
			m_pMeshCom->Set_AnimationSet(46);
			break;
		case RED_Attack1:
			m_fAnimSpeed = 2.5f;
			m_uiAni = 47;
			m_pMeshCom->Set_AnimationSet(47);
			break;
		case RED_Fight_Idle:
			m_uiAni = 48;
			m_pMeshCom->Set_AnimationSet(48);
			break;
		case RED_NoneFight_Idle:
			m_uiAni = 48;
			m_pMeshCom->Set_AnimationSet(29);
			break;
		case RED_RedDevil_End:
			break;
		default:
			break;
		}
	}
	

}

void CRedDevil::Battle_Start(_float fTimeDelta)
{
	if (m_eCurState == RED_NoneFight_Idle)
	{
		if (m_bIsStart)
		{
			m_pMeshCom->Set_AnimationSet(49);

			if (Get_AniRatio() >= 0.8f)
				m_eCurState = RED_Fight_Idle;
			return;
		}

		_float fDistance = Get_TargetDist();
		if (fDistance <= 5.f)
		{
			RotateToTarget(fTimeDelta, 0.f);
			m_bIsStart = true;
			m_pMeshCom->Set_AnimationSet(49);

			
		}
		else
		{

		}



	}

}

void CRedDevil::Pattern(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case RED_Death_B:
	case RED_Death_F:
		Death(fTimeDelta);
		break;
	case RED_DownEnd_P:
	case RED_DownEnd_S:
		Down_End(fTimeDelta);
		break;
	case RED_DashAttack_E:
		DashAttack_E(fTimeDelta);
		break;
	case RED_DashAttack_L:
		DashAttack_L(fTimeDelta);
		break;
	case RED_DashAttack_S:
		DashAttack_S(fTimeDelta);
		break;
	case RED_Attack3:
		Attack3(fTimeDelta);
		break;
	case RED_Attack2:
		Attack2(fTimeDelta);
		break;
	case RED_Attack1:
		Attack1(fTimeDelta);
		break;
	case RED_Fight_Idle:
		m_fAttackRange = 4.0f;
		Idle(fTimeDelta);
		break;
	case RED_NoneFight_Idle:
		m_fAttackRange = 5.0f;
		Battle_Start(fTimeDelta);
		break;
	case RED_RedDevil_End:
		break;
	default:
		m_fAnimSpeed = 2.0f;
		Idle(fTimeDelta);
		break;
	}
	
}


void CRedDevil::KnockBack(_float fTimeDelta)
{
	if (m_bIsHurt)
	{
		m_dwHurtDirectionFlag = 0;
		_vec3 vDir;
		ExtractY_NormalDir(Get_Pos(), Get_TargetPos(), &vDir);


		//cout << "X=" <<m_vHurtPos.x << "Y=" << m_vHurtPos.y << "Z=" << m_vHurtPos.z << endl;
		_vec3 vLook = Get_Pos();
		_vec3 vRight = Get_Right();
		_float fAngle = 0.f;
		_float fCos60 = 0.15425144988;

		fAngle = Get_Angle(vDir, vLook);
		

		if (cosf(D3DXToRadian(fAngle)) >= fCos60)
			m_dwHurtDirectionFlag |= FRONT;
		else if (cosf(D3DXToRadian(fAngle)) <= -fCos60)
			m_dwHurtDirectionFlag |= BACK;

		fAngle = Get_Angle(vDir, vRight);
		if (cosf(D3DXToRadian(fAngle)) >= fCos60)
			m_dwHurtDirectionFlag |= LEFT;
		else if (cosf(D3DXToRadian(fAngle)) <= -fCos60)
			m_dwHurtDirectionFlag |= RIGHT;


		if(m_fCurHp > 0.f )
		{
			switch (m_dwHurtDirectionFlag)
			{
			case DIR_F:
				m_bisStrongHurt ? m_eCurState = RED_Down_S : m_eCurState = RED_Damage_BR;
				break;
			case DIR_FR:
				m_bisStrongHurt ? m_eCurState = RED_Down_S : m_eCurState = RED_Damage_BL;
				break;
			case DIR_R:
				m_bisStrongHurt ? m_eCurState = RED_Down_S : m_eCurState = RED_Damage_BL;
				break;
			case DIR_B:
				m_bisStrongHurt ? m_eCurState = RED_Down_P : m_eCurState = RED_Damage_FL;
				break;
			case DIR_BR:
				m_bisStrongHurt ? m_eCurState = RED_Down_P : m_eCurState = RED_Damage_FL;
				break;
			case DIR_BL:
				m_bisStrongHurt ? m_eCurState = RED_Down_P : m_eCurState = RED_Damage_FR;
				break;
			case DIR_L:
				m_bisStrongHurt ? m_eCurState = RED_Down_S : m_eCurState = RED_Damage_BR;
				break;
			case DIR_FL:
				m_bisStrongHurt ? m_eCurState = RED_Down_S : m_eCurState = RED_Damage_BR;
				break;
				//default:
				//	break;
			}
		}
		else
		{
			switch (m_dwHurtDirectionFlag)
			{
			case DIR_F:
			case DIR_R:
			case DIR_FR:
			case DIR_L:
			case DIR_FL:
				m_eCurState = RED_Down_S;
				break;
			case DIR_BL:
			case DIR_B:
			case DIR_BR:
				m_eCurState = RED_Down_P;
				break;
			}
		}
		m_bIsHurt = false;
	}
}

void CRedDevil::Chaing_Target(_float fTimeDelta)
{
	if (m_pNaviCom != nullptr)
	{
			_vec3 vDir, vOutPos;
			_float	fDgree = Get_AngleOnTheTarget();
			vDir = Get_TargetPos() - Get_Pos();
			m_fDistance = D3DXVec3Length(&vDir);
			vDir.y = 0.f;
			D3DXVec3Normalize(&vDir, &vDir);
			RotateToTarget(fTimeDelta, 0.0f);

			m_pNaviCom->Move_OnNaviMesh(&Get_Pos(), &(vDir * m_fSpeed* fTimeDelta), &vOutPos);
			m_pTransformCom->Set_Pos(vOutPos.x, vOutPos.y, vOutPos.z);
	}

}


void CRedDevil::DashAttack_S(_float fTimeDelta)
{
	if (m_eCurState == RED_DashAttack_S)
	{
		//m_fRotSpeed = 3.0f;
		//RotateToTarget(fTimeDelta, 0.0f, 0.3f);

		m_fRotSpeed = 2.0f;
		m_fAnimSpeed = 1.5f;
		RotateToTarget(fTimeDelta, 0.73f);
		MoveAni(fTimeDelta, 0.f, 0.88, 2.f, Get_Look());
		//SetColliderEnable(0.6f, 0.88f);

		if (Get_AniRatio() >= 0.88f)
		{
				m_eCurState = RED_DashAttack_L;
		}

	}
}

void CRedDevil::DashAttack_L(_float fTimeDelta)
{

	if (m_eCurState == RED_DashAttack_L)
	{
		//SetColliderEnable(0.0f,);

		m_fAnimSpeed = 1.5f;
		m_fRotSpeed = 2.0f;

		RotateToTarget(fTimeDelta, 0.73f);
		MoveAni(fTimeDelta, 0.0f, 30.f, m_fSpeed*3.f, Get_Look());

		if (Get_TargetDist() <= 4.f)
		{
			m_eCurState = RED_DashAttack_E;
		}

		//RotateToTarget(fTimeDelta, 0.0f, 0.3f);
	}

}

void CRedDevil::DashAttack_E(_float fTimeDelta)
{

	if (m_eCurState == RED_DashAttack_E)
	{
		m_fAnimSpeed = 3.f;
		m_fRotSpeed = 2.0f;
		//RotateToTarget(fTimeDelta, 0.0f, 0.3f);
		MoveAni(fTimeDelta, 0.0f, 0.2f, 8.0f, Get_Look());

		SetColliderEnable(0.0f, 0.7f);

		if (Get_AniRatio() >= 0.8f)
		{
				m_eCurState = RED_Fight_Idle;
		}
	}

}

void CRedDevil::Attack1(_float fTimeDelta)
{
	if (m_eCurState == RED_Attack1)
	{
		m_fAttackRange = 7.f;
		SetColliderEnable(0.05f, 0.5f);

		if (Get_AniRatio() >= 0.65f)
		{
			m_fRotSpeed = 2.0f;
			if(m_fAttackRange>=m_fDistance)
				m_eCurState = RED_Attack2;
			else
				m_eCurState = RED_Fight_Idle;


		}
		else
		{
			//m_fAnimSpeed = 3.f;
			RotateToTarget(fTimeDelta, 0.f, 0.2f);
			MoveAni(fTimeDelta, 0.2f, 0.275f, 5.0f, Get_Look());

		}
	}
}

void CRedDevil::Attack2(_float fTimeDelta)
{
	if (m_eCurState == RED_Attack2)
	{
		m_fAttackRange = 7.f;

		SetColliderEnable(0.2f,0.4f);

		if (Get_AniRatio() >= 0.65f)
		{
			if (m_fAttackRange >= m_fDistance)
				m_eCurState = RED_Attack3;
			else
				m_eCurState = RED_Fight_Idle;


		}
		else
		{
			m_fAnimSpeed = 3.f;
			RotateToTarget(fTimeDelta, 0.f, 0.2f);
			MoveAni(fTimeDelta, 0.2f, 0.3f, 4.0f, Get_Look());

		}
	}
}

void CRedDevil::Attack3(_float fTimeDelta)
{
	if (m_eCurState == RED_Attack3)
	{
		SetColliderEnable(0.1f, 0.5f);

		if (Get_AniRatio() >= 0.8f)
		{
			m_eCurState = RED_Fight_Idle;
		}
		else
		{
			m_fAnimSpeed = 3.f;
			RotateToTarget(fTimeDelta, 0.f, 0.2f);
			MoveAni(fTimeDelta, 0.15f, 0.225f, 4.0f, Get_Look());

		}
	}

}



void CRedDevil::Idle(_float fTimeDelta)
{
	if (m_eCurState == RED_Fight_Idle|| m_eCurState == RED_Run_F)
	{
		if (m_fDistance >= m_fAttackRange)
		{
			if (m_fDistance >= 8.f)
			{
				m_fSpeed = 3.f;
				RotateToTarget(fTimeDelta, 0.0f);
				m_eCurState = RED_DashAttack_S;
			}
			else
			{
				m_fSpeed = 3.f;

				if(m_eCurState == RED_Run_F)
					Chaing_Target(fTimeDelta);

				RotateToTarget(fTimeDelta, 0.0f);
				m_eCurState = RED_Run_F;

			}

		}
		else
		{
			m_eCurState = RED_Attack1;
			//m_eCurState = RED_DashAttack_S;

		}
	
	}

	
}

void CRedDevil::Collision_Check(_float fTimeDelta)
{
	Engine::CColliderGroup* pTargetCollCom = nullptr;
	Engine::CTransform* pTargetTransCom = nullptr;
	for (auto &pObject : *m_ppGameObjectMap)
	{
		if (!pObject.second->IsMonster())
			continue;

		pTargetTransCom = dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"GameLogic",
																pObject.second->Get_InstName().c_str(),
																L"Com_Transform",
																Engine::ID_DYNAMIC));
		_vec3 vPos, vTargetPos , vDir;
		vPos = Get_Pos();
		vTargetPos = *pTargetTransCom->Get_Info(Engine::INFO_POS);
		vDir = vPos - vTargetPos;
		_float fLen = D3DXVec3Length(&vDir);
		
		if (fLen >= 5.f)
			continue;

		pTargetCollCom = dynamic_cast<Engine::CColliderGroup*>(Engine::Get_Component(L"GameLogic",
																pObject.second->Get_InstName().c_str(),
																L"Com_ColliderGroup",
																Engine::ID_DYNAMIC));
		if (pTargetCollCom != nullptr)
		{
			_float fPower = 0.f;
			_bool bIsStepColl = false;
			_vec3 vOutPos;

			bIsStepColl = m_pCalculatorCom->Bounding_Sphere(m_pColliderGroupCom->Get_CollVec(Engine::COLOPT_STEP),
				pTargetCollCom->Get_CollVec(Engine::COLOPT_STEP), &fPower);
	
			if (bIsStepColl)
			{
				pTargetTransCom = dynamic_cast<Engine::CTransform*>(pObject.second->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC));

				vPos = *m_pTransformCom->Get_Info(Engine::INFO_POS);
				vTargetPos = *pTargetTransCom->Get_Info(Engine::INFO_POS);

				D3DXVec3Normalize(&vDir, &vDir);
				vDir = vDir*fPower;

				m_pNaviCom->Move_OnNaviMesh(&vPos, &vDir, &vOutPos);

				m_pTransformCom->Set_Pos(vOutPos.x, vOutPos.y, vOutPos.z);	//적용
			}

		}


	}
}

void CRedDevil::HurtMoveSet(_float fTimeDelta)
{
	_vec3 vDir = Get_Pos() - Get_TargetPos();
	D3DXVec3Normalize(&vDir, &vDir);
	if (m_eCurState == RED_Down_S || m_eCurState == RED_Down_P)
	{
		if (Get_AniRatio() >= 0.85f)
		{
			if (m_fCurHp > 0.f)
				m_eCurState == RED_Down_S ? m_eCurState = RED_DownEnd_S : m_eCurState = RED_DownEnd_P;
			else
			{
				m_eCurState == RED_Down_S ? m_eCurState = RED_Death_B : m_eCurState = RED_Death_F;

			}
		
		}
	}
	else
	{
		if (Get_AniRatio() >= 0.85f)
			m_eCurState = RED_Fight_Idle;
	}
	switch (m_eCurState)
	{
	case RED_Down_S:
	case RED_Down_P:
		MoveAni(fTimeDelta, 0.f, 0.15f, 5.f, vDir*2.f);
		break;
	case RED_Damage_FR:
	case RED_Damage_FL:
	case RED_Damage_BR:
	case RED_Damage_BL:
		MoveAni(fTimeDelta, 0.f, 0.15f, 3.f, vDir);
		break;
	}
			
}

void CRedDevil::Down_End(_float fTimeDelta)
{

	if (Get_AniRatio() >= 0.8f)
	{
		if(m_fCurHp>0.f)
			m_eCurState = RED_Fight_Idle;
		
	}
}

void CRedDevil::Death(_float fTimeDelta)
{
	if (Get_AniRatio() > 0.8f)
	{
		m_bIsMonDead = true;
	
		for (int i = 0; i < Engine::COLOPT_END; i++)
			m_pColliderGroupCom->Set_ColliderEnable((Engine::COLLOPTION)i, false);
	
	}
}

CRedDevil * CRedDevil::Create(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstrName,_uint uiIdx, _uint uiStageIdx)
{
	CRedDevil*	pInstance = new CRedDevil(pGraphicDev, wstrName, uiIdx, uiStageIdx);

	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}
CRedDevil * CRedDevil::Create(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstrName, _uint uiIdx, TRANSFORM_INFO tInfo, _uint uiStageIdx)
{
	CRedDevil*	pInstance = new CRedDevil(pGraphicDev, wstrName, uiIdx, tInfo, uiStageIdx);

	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;

}
void CRedDevil::Free(void)
{
	Engine::CGameObject::Free();
}
