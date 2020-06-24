#ifndef Halberd_h__
#define Halberd_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CStaticMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;
class CShader;

END

class CHalberd : public Engine::CGameObject
{
private:
	explicit				CHalberd(LPDIRECT3DDEVICE9 pGraphicDev, _uint uiInstIdx);
	virtual					~CHalberd(void);

public:

public:
	HRESULT					Ready_GameObject();
	virtual _int			Update_GameObject(const _float& fTimeDelta) override;
	virtual void			Render_GameObject(void) override;
	HRESULT					SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:
	HRESULT					Add_Component(void);

public:

	_bool					Get_Equip() { return m_bIsEquip; }
	void					Set_Equip(_bool bIsEquip);
	void					Set_Throw();

private:
	Engine::CTransform*		m_pTransformCom = nullptr;
	Engine::CRenderer*		m_pRendererCom = nullptr;
	Engine::CCalculator*	m_pCalculatorCom = nullptr;
	Engine::CStaticMesh*	m_pMeshCom = nullptr;
	Engine::CCollider*		m_pColliderCom = nullptr;
	Engine::CShader*		m_pShaderCom = nullptr;


	_vec3					m_vThrow = { INIT_VEC3 };
	_float					m_fAccThrow = 4.f;
	_uint					m_iFlag = 0;
	_bool					m_bColl = false;
	_bool					m_bIsEquip = false;
	_vec3					m_vOldPos;
	_vec3					m_vOldRotation;
	_matrix					m_MatOldWorld;
	const	_matrix*		m_pParentBoneMatrix = nullptr;
	const	_matrix*		m_pParentWorldMatrix = nullptr;
	wstring					m_wstrEquipName;


public:
	static CHalberd*		Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint uiInstIdx);

private:
	virtual void Free(void) override;
};

#endif // Halberd_h__
