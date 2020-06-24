#include "StaticMesh.h"

USING(Engine)

Engine::CStaticMesh::CStaticMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMesh(pGraphicDev)
{

}

Engine::CStaticMesh::CStaticMesh(const CStaticMesh& rhs)
	: CMesh(rhs)
	, m_pMesh(rhs.m_pMesh)
	, m_pAdjacency(rhs.m_pAdjacency)
	, m_pSubsetBuffer(rhs.m_pSubsetBuffer)
	, m_pMtrl(rhs.m_pMtrl)
	, m_dwSubsetCnt(rhs.m_dwSubsetCnt)
	, m_dwStride(rhs.m_dwStride)
	, m_dwNumVtx(rhs.m_dwNumVtx)
	, m_pVtxPos(rhs.m_pVtxPos)
	, m_pOriMesh(rhs.m_pOriMesh)
{
	m_ppTexture = new LPDIRECT3DTEXTURE9[rhs.m_dwSubsetCnt];

	for (_ulong i = 0; i < rhs.m_dwSubsetCnt; ++i)
	{
		m_ppTexture[i] = rhs.m_ppTexture[i];
		m_ppTexture[i]->AddRef();
	}

	m_ppNormalTexture = new LPDIRECT3DTEXTURE9[rhs.m_dwSubsetCnt];

	for (_ulong i = 0; i < rhs.m_dwSubsetCnt; ++i)
	{
		if (nullptr == rhs.m_ppNormalTexture[i])
		{
			m_ppNormalTexture[i] = nullptr;
		}
		else
		{
			m_ppNormalTexture[i] = rhs.m_ppNormalTexture[i];
			m_ppNormalTexture[i]->AddRef();
		}
	}

	m_pMesh->AddRef();
	m_pOriMesh->AddRef();
	m_pAdjacency->AddRef();
	m_pSubsetBuffer->AddRef();
}

Engine::CStaticMesh::~CStaticMesh(void)
{

}

_bool CStaticMesh::Find_Alpha(const char * pFileName)
{
	_uint	iLength = strlen(pFileName);

	for (_uint i = 0; i < iLength; ++i)
	{
		if (pFileName[i] == '.')
		{
			if (pFileName[i - 1] == 'A')
				return true;
		}
	}

	return false;
}

HRESULT Engine::CStaticMesh::Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName)
{
	_tchar		szFullPath[256] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	FAILED_CHECK_RETURN(D3DXLoadMeshFromX(szFullPath,
		D3DXMESH_MANAGED,
		m_pGraphicDev,
		&m_pAdjacency,
		&m_pSubsetBuffer,
		NULL,
		&m_dwSubsetCnt,
		&m_pOriMesh),
		E_FAIL);

	D3DVERTEXELEMENT9		Decl[MAX_FVF_DECL_SIZE];
	ZeroMemory(Decl, sizeof(D3DVERTEXELEMENT9) * MAX_FVF_DECL_SIZE);

	m_pOriMesh->GetDeclaration(Decl);
	m_pOriMesh->CloneMesh(m_pOriMesh->GetOptions(), Decl, m_pGraphicDev, &m_pMesh);

	m_pMtrl = (D3DXMATERIAL*)m_pSubsetBuffer->GetBufferPointer();

	m_ppTexture = new LPDIRECT3DTEXTURE9[m_dwSubsetCnt];
	m_ppNormalTexture = new LPDIRECT3DTEXTURE9[m_dwSubsetCnt];

	for (_ulong i = 0; i < m_dwSubsetCnt; ++i)
	{
		_tchar	szFileName[256] = L"";
		lstrcpy(szFullPath, pFilePath);

		MultiByteToWideChar(CP_ACP, 0, m_pMtrl[i].pTextureFilename, strlen(m_pMtrl[i].pTextureFilename), szFileName, 256);

		lstrcat(szFullPath, szFileName);

		FAILED_CHECK_RETURN(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_ppTexture[i]), E_FAIL);

		if (!LoadNormalTexture(szFullPath, &m_ppNormalTexture[i]))
		{
			m_ppNormalTexture[i] = nullptr;
		}
	}

	return S_OK;
}

_bool CStaticMesh::LoadNormalTexture(const _tchar * szFilePath, LPDIRECT3DTEXTURE9 * ppTexture)
{
	wstring wstrKey = szFilePath;

	_int iNum = 0;
	for (int i = wstrKey.size() - 1; i >= 0; --i)
	{
		if (wstrKey[i] == L'.')
		{
			iNum = i;
			break;
		}
	}

	if (0 == iNum)
		return false;

	wstrKey[iNum - 1] = L'N';
	if (D3D_OK == D3DXCreateTextureFromFile(m_pGraphicDev, wstrKey.c_str(), ppTexture))
	{
		return true;
	}

	return false;
}

void Engine::CStaticMesh::Render_Meshes(void)
{
	for (_ulong i = 0; i < m_dwSubsetCnt; ++i)
	{
		m_pGraphicDev->SetTexture(0, m_ppTexture[i]);
		m_pMesh->DrawSubset(i);
	}
}

void CStaticMesh::Render_Meshes(LPD3DXEFFECT & pEffect)
{
	for (_ulong i = 0; i < m_dwSubsetCnt; ++i)
	{
		//_bool	bAlpha = false;
		//_uint	iPass = 0;

		//if (bAlpha = Find_Alpha(m_pMtrl[i].pTextureFilename))
		//	iPass = 1;

		pEffect->SetTexture("g_NormalTexture", m_ppNormalTexture[i]);
		pEffect->SetTexture("g_BaseTexture", m_ppTexture[i]);
		pEffect->CommitChanges();
		m_pMesh->DrawSubset(i);

		//pEffect->BeginPass(iPass);
		//pEffect->EndPass();


	}
}

Engine::CStaticMesh* Engine::CStaticMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName)
{
	CStaticMesh*	pInstance = new CStaticMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_Meshes(pFilePath, pFileName)))
		Safe_Release(pInstance);

	return pInstance;
}

Engine::CResources* Engine::CStaticMesh::Clone(void)
{
	return new CStaticMesh(*this);
}

void Engine::CStaticMesh::Free(void)
{
	_bool bis = m_bClone;
	CMesh::Free();

	for (_ulong i = 0; i < m_dwSubsetCnt; ++i)
	{
		Safe_Release(m_ppTexture[i]);
		Safe_Release(m_ppNormalTexture[i]);
	}

	Safe_Delete_Array(m_ppTexture);
	Safe_Delete_Array(m_ppNormalTexture);

	if (false == m_bClone)
		Safe_Delete_Array(m_pVtxPos);

	Safe_Release(m_pAdjacency);
	Safe_Release(m_pSubsetBuffer);
	Safe_Release(m_pMesh);
	Safe_Release(m_pOriMesh);

}

