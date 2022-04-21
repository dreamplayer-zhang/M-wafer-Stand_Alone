#include "StdAfx.h"
#include "INI.h"

bool CINI::Load(CString strPath, CString strCaption, CString strId, bool &bValue)
{
	if (strPath   .IsEmpty()) return false;
	if (strCaption.IsEmpty()) return false;
	if (strId     .IsEmpty()) return false;
	if (IsExistDir(strPath) == false) return false;

	TCHAR sDir[MAX_PATH] = {0,};
	CString str=_T("");

	GetPrivateProfileString(strCaption, strId, _T("false"), sDir, MAX_PATH, strPath);
	str.Format(_T("%s"),sDir);
	bValue = (str == _T("true")) ? true : false;

	return true;
}

bool CINI::Save(CString strPath, CString strCaption, CString strId, bool bValue)
{
	if (strPath   .IsEmpty()) return false;
	if (strCaption.IsEmpty()) return false;
	if (strId     .IsEmpty()) return false;
	//if (IsExistDir(strPath) == false) return false;

	CString str = (bValue) ? _T("true") : _T("false");

	WritePrivateProfileString(strCaption, strId, str, strPath);

	return true;
}

bool CINI::Load(CString strPath, CString strCaption, CString strId, int &nValue)
{
	if (strPath   .IsEmpty()) return false;
	if (strCaption.IsEmpty()) return false;
	if (strId     .IsEmpty()) return false;
	if (IsExistDir(strPath) == false) return false;

	TCHAR sDir[MAX_PATH] = {0,};

	GetPrivateProfileString(strCaption, strId, _T("0"), sDir, MAX_PATH, strPath);
	nValue = _ttoi(sDir);

	return true;
}

bool CINI::Save(CString strPath, CString strCaption, CString strId, int nValue)
{
	if (strPath   .IsEmpty()) return false;
	if (strCaption.IsEmpty()) return false;
	if (strId     .IsEmpty()) return false;
	//if (IsExistDir(strPath) == false) return false;

	TCHAR sDir[MAX_PATH] = {0,};
	CString strTmp=_T("");

	strTmp.Format(_T("%d"),nValue);
	WritePrivateProfileString(strCaption, strId, strTmp, strPath);

	return true;
}

bool CINI::Load(CString strPath, CString strCaption, CString strId, float &fValue)
{
	if (strPath   .IsEmpty()) return false;
	if (strCaption.IsEmpty()) return false;
	if (strId     .IsEmpty()) return false;
	if (IsExistDir(strPath) == false) return false;

	TCHAR sDir[MAX_PATH] = {0,};

	GetPrivateProfileString(strCaption, strId, _T("0"), sDir, MAX_PATH, strPath);
	fValue = (float) _tstof(sDir);

	return true;
}

bool CINI::Save(CString strPath, CString strCaption, CString strId, float fValue)
{
	if (strPath   .IsEmpty()) return false;
	if (strCaption.IsEmpty()) return false;
	if (strId     .IsEmpty()) return false;
	//if (IsExistDir(strPath) == false) return false;

	TCHAR sDir[MAX_PATH] = {0,};
	CString strTmp=_T("");

	strTmp.Format(_T("%.5f"),fValue);
	WritePrivateProfileString(strCaption, strId, strTmp, strPath);

	return true;
}

bool CINI::Load(CString strPath, CString strCaption, CString strId, double &dValue)
{
	if (strPath   .IsEmpty()) return false;
	if (strCaption.IsEmpty()) return false;
	if (strId     .IsEmpty()) return false;
	if (IsExistDir(strPath) == false) return false;

	TCHAR sDir[MAX_PATH] = {0,};

	GetPrivateProfileString(strCaption, strId, _T("0"), sDir, MAX_PATH, strPath);
	dValue = (double) _tstof(sDir);

	return true;
}

bool CINI::Save(CString strPath, CString strCaption, CString strId, double dValue)
{
	if (strPath   .IsEmpty()) return false;
	if (strCaption.IsEmpty()) return false;
	if (strId     .IsEmpty()) return false;
	//if (IsExistDir(strPath) == false) return false;

	CString strTmp=_T("");

	strTmp.Format(_T("%.5f"),dValue);
	WritePrivateProfileString(strCaption, strId, strTmp, strPath);

	return true;
}

bool CINI::Load(CString strPath, CString strCaption, CString strId, CString &strValue)
{
	if (strPath   .IsEmpty()) return false;
	if (strCaption.IsEmpty()) return false;
	if (strId     .IsEmpty()) return false;
	if (IsExistDir(strPath) == false) return false;

	TCHAR sDir[MAX_PATH] = {0,};

	GetPrivateProfileString(strCaption, strId, _T(""), sDir, MAX_PATH, strPath);
	strValue = (CString)sDir;

	return true;
}

bool CINI::Save(CString strPath, CString strCaption, CString strId, CString strValue)
{
	if (strPath   .IsEmpty()) return false;
	if (strCaption.IsEmpty()) return false;
	if (strId     .IsEmpty()) return false;
	//if (IsExistDir(strPath) == false) return false;

	WritePrivateProfileString(strCaption, strId, strValue, strPath);

	return true;
}