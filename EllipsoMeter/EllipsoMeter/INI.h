//----------------------------------------------------------
// INI Class
//----------------------------------------------------------
// Programmed by William Kim
//----------------------------------------------------------
// Last Update : 2016-11-29 11:37
// Modified by William Kim
//----------------------------------------------------------

#pragma once

class CINI
{
public :
	static bool Load(CString strPath, CString strCaption, CString strId, bool &bValue);
	static bool Save(CString strPath, CString strCaption, CString strId, bool bValue);
	static bool Load(CString strPath, CString strCaption, CString strId, int &nValue);
	static bool Save(CString strPath, CString strCaption, CString strId, int nValue);
	static bool Load(CString strPath, CString strCaption, CString strId, float &fValue);
	static bool Save(CString strPath, CString strCaption, CString strId, float fValue);
	static bool Load(CString strPath, CString strCaption, CString strId, double &dValue);
	static bool Save(CString strPath, CString strCaption, CString strId, double dValue);
	static bool Load(CString strPath, CString strCaption, CString strId, CString &strValue);
	static bool Save(CString strPath, CString strCaption, CString strId, CString strValue);
};

static bool IsExistDir(CString strPath) 
{ 
	if (GetFileAttributes(strPath) == -1) return false; 
	
	return true; 
} // -1 is No Exist