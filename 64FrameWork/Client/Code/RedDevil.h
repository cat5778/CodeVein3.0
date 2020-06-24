#pragma once
#include "Defines.h"
#include "GameObject.h"
#include "DynamicObject.h"
BEGIN(Engine)
class CNaviMesh;
END
class CRedDevil : public CDynamicObject
{
public:
	explicit				CRedDevil(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstrName, _uint uiIdx,_uint uiStageIdx=1);
	explicit				CRedDevil(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstrName, _uint uiIdx, TRANSFORM_INFO tInfo, _uint uiStageIdx=1);
	virtual					~CRedDevil(void);

public:
	HRESULT					Ready_GameObject();
	virtual	HRESULT			LateReady_GameObject()override;
	virtual _int			Update_GameObject(const _float& fTimeDelta) override;
	virtual void			Render_GameObject(void) override;
	


private:
	virtual HRESULT			Add_Component(void);
	void					Set_TransformData();
	void					StateMachine()override;

private://Pattern
	void					Battle_Start(_float fTimeDelta);
	void					Pattern(_float fTimeDelta);


	void					KnockBack(_float fTimeDelta);
	void					Chaing_Target(_float fTimeDelta);

	void					DashAttack_S(_float fTimeDelta);
	void					DashAttack_L(_float fTimeDelta);
	void					DashAttack_E(_float fTimeDelta);

	void					Attack1(_float fTimeDelta);
	void					Attack2(_float fTimeDelta);
	void					Attack3(_float fTimeDelta);

	void					Idle(_float fTimeDelta);
	void					Collision_Check(_float fTimeDelta);
	void					HurtMoveSet(_float fTimeDelta);
	void					Down_End(_float fTimeDelta);
	void					Death(_float fTimeDelta);

public:
	static CRedDevil*	Create(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstrName, _uint uiIdx = 0, _uint uiInstIdx = 0);
	static CRedDevil*	Create(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstrName, _uint uiIdx ,  TRANSFORM_INFO tInfo, _uint uiStageIdx = 0);



private:
	_bool						m_bIsStart = false;
	_float						m_fJump=0.f;
	REDDEVILSTATE				m_eCurState;
	REDDEVILSTATE				m_ePreState;
	DWORD						m_dwHurtDirectionFlag = 0;



private:
	virtual void			Free(void) override;
};

/* RedDevil Ani Index */
//30. NoneFight_Idle_L
//31. Death_F
//32. Death_B
//33. DownEnd_S
//34. DownEnd_P
//35. Down_S
//36. Down_P
//37. Damage_FR
//38. Damage_FL
//39. Damage_BR
//40. Damage_BL
//41. Run_F
//42. DashAttack_E
//43. DashAttack_L
//44. DashAttack_S
//45. Attack3
//46. Attack2
//47. Attack1
//48. Fight_Idle
//49. NoneFight_Idle_E


/*CoCoonDevil
44. Damage_B
45. Damage_F
46. Death
47. Idle
48. Attack_Shoot_N
49. Attack_Mist_N
*/
