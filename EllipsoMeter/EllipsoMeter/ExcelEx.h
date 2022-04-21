#pragma once

#include "EXCEL\\CApplication.h"
#include "EXCEL\\CBorder.h"
#include "EXCEL\\CBorders.h"
#include "EXCEL\\CPicture.h"
#include "EXCEL\\CPictures.h"
#include "EXCEL\\CRange.h"
#include "EXCEL\\CShapes.h"
#include "EXCEL\\CWorkbook.h"
#include "EXCEL\\CWorkbooks.h"
#include "EXCEL\\CWorksheet.h"
#include "EXCEL\\CWorksheets.h"

class CExcelEx
{
public:
	CExcelEx(void);
	~CExcelEx(void);
protected:
	// spreadsheet variables
	CApplication	m_app;
	CWorkbook		m_book;
	CWorkbooks		m_books;
	CWorksheet		m_sheet;
	CWorksheets		m_sheets;
	COleVariant		m_covTrue;
	COleVariant		m_covFalse;
	COleVariant		m_covOptional;
	void usrTerminateProcess(CString strImageName);
	CString GetExcelPos(int nCol, int nRow);
public:
	CString         m_strMsg;
	BOOL			m_bIsExcelStarted;
	BOOL			m_bIsSheetSelected;
	void CloseWorkBook(void);
	void OpenWorkBook(CString strFilePath);
	void CreateWorkBook(CString strFilePath);
	void SelectSheet(int nNum);
	void SelectSheet(CString strName);
	void SetSheetName(CString strName);
	void AddSheet(CString strName= "");
	void SaveData(void);
	void SaveAs(CString strFilePath);
	int  InsertPicture(CString strTopLeft, CString strBottomRight, CString strPicPath);
	void InsertString(int nCol, int nRow, CString strValue, BOOL bBorder = false);
	void ViewSheetNamesExist(CComboBox* pCbBox);
	void StartExcel(void);
	void QuitExcel(BOOL bExit = FALSE);
	void ShowExcel(BOOL bShow);
	CString ReadData(int nCol, int nRow);
	void SetPrintOut();
	void CellBackGroundColor(int nCol, int nRow, CString strValue, COLORREF Color , BOOL bBorder = false);

protected:
	void Initialize(void);
};
