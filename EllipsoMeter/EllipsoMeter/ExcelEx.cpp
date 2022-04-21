#include "stdafx.h"
#include "ExcelEx.h"
#include "tlhelp32.h"

const short xlBorderWeightThin = 2;
const double dbThresholdPicture = 1.333;

CExcelEx::CExcelEx(void)
{
	Initialize();
}


CExcelEx::~CExcelEx(void)
{
	QuitExcel(TRUE);
}

void CExcelEx::SaveData(void)
{
	TRY
	{
		// save
		m_book.Save();
	}
	//Clean up if something went wrong.
	CATCH(CException, e)
	{
		m_strMsg = "Could not save data";
		TECmsgOut( m_Util.CStringTobstr( m_strMsg ), RED_1, 1 );
	}
	END_CATCH
}

void CExcelEx::SaveAs(CString strFilePath)
{
	TRY
	{
		// save
		m_book.SaveAs(COleVariant(strFilePath),
			m_covOptional,m_covOptional,m_covOptional,m_covOptional,
			m_covOptional,1,m_covOptional,m_covOptional,
			m_covOptional,m_covOptional);
	}
	//Clean up if something went wrong.
	CATCH(CException, e)
	{
		m_strMsg = "Could not save data";
		TECmsgOut(m_Util.CStringTobstr( m_strMsg), RED_1, 1);
	}
	END_CATCH
}

void CExcelEx::InsertString(int nCol, int nRow, CString strValue, BOOL bBorder)
{
	CRange range;
	CBorder border;
	CString strPos = GetExcelPos(nCol,nRow);

	if(m_bIsSheetSelected==TRUE)
	{
		range = m_sheet.get_Range(COleVariant(strPos), COleVariant(strPos));
		range.Select();
	
		if ( bBorder )
		{
			range.get_Font();
		    border = range.get_Borders();
		    border.put_Weight(COleVariant(xlBorderWeightThin));
		}
		range.put_Value(COleVariant(strValue));
	}
	else
	{
		m_strMsg = "Sheet is not selected. Select a sheet first.";
		TECmsgOut( m_Util.CStringTobstr( m_strMsg), GREEN_1, 1);
	}
}

void CExcelEx::CellBackGroundColor(int nCol, int nRow, CString strValue, COLORREF Color, BOOL bBorder )
{
	CRange range;
	CBorder border;
	CString strPos = GetExcelPos(nCol,nRow);

	if(m_bIsSheetSelected==TRUE)
	{
		range = m_sheet.get_Range(COleVariant(strPos), COleVariant(strPos));
		range.Select();
	
		range.get_Font();
		if ( bBorder )
		{
			border = range.get_Borders();
			border.put_Weight(COleVariant(xlBorderWeightThin));
		}
		border = range.get_Interior();
		border.put_Color(COleVariant((double)Color));
		range.put_Value(COleVariant(strValue));
	}
	else
	{
		m_strMsg = "Sheet is not selected. Select a sheet first.";
		TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED_1, 1);
	}
}

CString CExcelEx::GetExcelPos(int nCol, int nRow)
{
	CString strRet;
	int nDiv=0, nMod=0;
	if(nCol>26)
	{
		nDiv = nCol / 26;
		nMod = nCol % 26;
		strRet.Format("%c%c%d", 0x40+nDiv, 0x40+nMod, nRow);
	}
	else
	{
		strRet.Format("%c%d", 0x40+nCol, nRow);
	}
	return strRet;
}

int CExcelEx::InsertPicture( CString strTopLeft, CString strBottonRight, CString strPicPath)
{
	if(m_bIsSheetSelected==TRUE)
	{
		double      dLeft   = 0; 
		double 	    dTop    = 0; 
		double 	    dWidth  = 0; 
		double 	    dHeight = 0;

		CRange		range;
		CShapes     shapes;
		VARIANT    var;

		LPDISPATCH lpDispShape = m_sheet.get_Shapes();
		shapes.AttachDispatch(lpDispShape);
		range = m_sheet.get_Range(COleVariant(strTopLeft), COleVariant(strBottonRight));

		var = range.get_Top();
		dLeft = (int)var.dblVal;
		var = range.get_Left();
		dTop = (int)var.dblVal;
		var = range.get_Width();
		dWidth = (int)var.dblVal;
		var = range.get_Height();
		dHeight = (int)var.dblVal;

		shapes.AddPicture( strPicPath,
							FALSE,
							TRUE,
							(float)dTop,
							(float)dLeft,
							(float)dWidth,
							(float)dHeight );

		shapes.ReleaseDispatch();

		// jump to the top of the sheet
		range = m_sheet.get_Range(COleVariant("A1"), COleVariant("A1"));
		range.Show ();
	}
	else
	{
		m_strMsg = "Sheet is not selected. Select a sheet first.";
		TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED_1, 1);
		return -1;
	}
	return 0;
}

void CExcelEx::CloseWorkBook(void)
{
	TRY
	{
		// close already-opened workbook
		m_book.ReleaseDispatch();
		m_books.Close();
		m_books.ReleaseDispatch();
		m_bIsSheetSelected=FALSE;
	}
	//Clean up if something went wrong.
	CATCH(CException, e)
	{
		m_strMsg = "Could not clean up workbook.";
		TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED_1, 1);
	}
	END_CATCH
}


void CExcelEx::OpenWorkBook(CString strFilePath)
{
	// close already-opened workbook
	CloseWorkBook();

	// open the specified workbook
	TRY
	{
		if(m_bIsExcelStarted==TRUE)
		{
			// Get Workbooks collection.
			LPDISPATCH lpDisp;
			lpDisp = m_app.get_Workbooks();  // Get an IDispatch pointer
			ASSERT(lpDisp);
			m_books.AttachDispatch( lpDisp );  // Attach the IDispatch pointer to the books object.

			// open the document
			lpDisp = m_books.Open(strFilePath,
							m_covOptional, m_covOptional, m_covOptional, m_covOptional,
							m_covOptional, m_covOptional, m_covOptional, m_covOptional,
							m_covOptional, m_covOptional, m_covOptional, m_covOptional
							);
			ASSERT(lpDisp);

			//Set CWorkbook to use lpDisp, the IDispatch* of the actual workbook.
			m_book.AttachDispatch(lpDisp);
		}
		else
		{
			m_strMsg = "Excel is not started. Start Excel first.";
			TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED_1, 1);
		}
	}
	//Clean up if something went wrong.
	CATCH(CException, e)
	{
		m_strMsg = "Could not open workbook.";
		TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED_1, 1);
		Initialize();
		return;
	}
	END_CATCH
}

void CExcelEx::CreateWorkBook(CString strFilePath)
{
	// close already-opened workbook
	CloseWorkBook();

	// open the specified workbook
	TRY
	{
		if(m_bIsExcelStarted==TRUE)
		{
			// Get Workbooks collection.
			LPDISPATCH lpDisp;
			lpDisp = m_app.get_Workbooks();  // Get an IDispatch pointer
			ASSERT(lpDisp);
			m_books.AttachDispatch( lpDisp );  // Attach the IDispatch pointer to the books object.
			
			// Create the document
			lpDisp = m_books.Add(m_covOptional);
			ASSERT(lpDisp);

			// Set CWorkbook to use lpDisp, the IDispatch* of the actual workbook.
			m_book.AttachDispatch(lpDisp);

			m_book.SaveAs(COleVariant(strFilePath),
				m_covOptional,m_covOptional,m_covOptional,m_covOptional,
				m_covOptional,1,m_covOptional,m_covOptional,
				m_covOptional,m_covOptional);
		}
		else
		{
			m_strMsg = "Excel is not started. Start Excel first.";
			TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED_1, 1);
		}
	}
	//Clean up if something went wrong.
	CATCH(CException, e)
	{
		m_strMsg = "Could not create workbook.";
		TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED_1, 1);
		Initialize();
		return;
	}
	END_CATCH
}

void CExcelEx::usrTerminateProcess(CString strImageName)
{
	HANDLE         hProcessSnap = NULL;
	BOOL           bRet = FALSE;
	PROCESSENTRY32 pe32 = { 0 };

	//  Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return;

	//  Fill in the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	//  Walk the snapshot of the processes, and for each process,
	//  display information.

	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			BOOL bTerminate = FALSE;
			if (strncmp(pe32.szExeFile, strImageName, strImageName.GetLength()) == 0)
			{
				bTerminate = TRUE;
			}
			else
			{
				int nCount = lstrlen(pe32.szExeFile), nCount2 = lstrlen(strImageName) + 1;
				char szExeFile[255] = { 0, };

				for (; ; )
				{
					if (nCount2 == -1) break;
					szExeFile[nCount2] = pe32.szExeFile[nCount];
					nCount2--;
					nCount--;
				}
				if (szExeFile[0] == '\\')
				{
					strcpy_s(szExeFile, &(szExeFile[1]));
				}
				if (strncmp(szExeFile, strImageName,strImageName.GetLength()) == 0)
				{
					bTerminate = TRUE;
				}
			}

			if (bTerminate)
			{
				// terminate
				HANDLE hProcess = NULL;
				if (hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID))
				{
					bRet = TerminateProcess(hProcess, 0);
					CloseHandle(hProcess);
				}
			}
		} while (Process32Next(hProcessSnap, &pe32));
		bRet = TRUE;
	}
	else
		bRet = FALSE;    // could not walk the list of processes
	// Do not forget to clean up the snapshot object.

	CloseHandle(hProcessSnap);
}

void CExcelEx::AddSheet(CString strName)
{
	TRY
	{
		CWorksheets sheets=m_book.get_Worksheets();
		CWorksheet sheetLast = sheets.get_Item(COleVariant((short)(sheets.get_Count()))); // sheets are indexed starting from 1
		CWorksheet sheetFirst = sheets.get_Item(COleVariant(1L)); // sheets are indexed starting from 1
		COleVariant vDefault;
		vDefault.vt = VT_DISPATCH;
		vDefault.pdispVal = (LPDISPATCH)sheetLast;

		CWorksheet sheetAdded = sheets.Add(m_covOptional, vDefault, COleVariant(1L),COleVariant(-4167L));
		if(strName!="")
			sheetAdded.put_Name(strName);
		sheetFirst.Activate();
	}
	//Clean up if something went wrong.
	CATCH(CException, e)
	{
		m_strMsg = "Could not add sheet";
		TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED_1, 1);
	}
	END_CATCH
}

void CExcelEx::ViewSheetNamesExist(CComboBox* pCbBox)
{
	// populate combo fields
	TRY
	{
		CWorksheets sheets;
		CWorksheet sheet;
		sheets = m_book.get_Worksheets();

		// get all the worksheet names and populate the combo box
		int count = sheets.get_Count();
		if (count)
		{
			for (int i = 0; i < count; i++)
			{
				sheet = sheets.get_Item(COleVariant((short)(i+1))); // sheets are indexed starting from 1
				CString name = sheet.get_Name ();
				pCbBox->AddString (name);
			}
			pCbBox->SetCurSel(0);
			//pCbBox->EnableWindow();
		}
	}
	//Clean up if something went wrong.
	CATCH(CException, e)
	{
		m_strMsg = "Could not get sheet names.";
		TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED_1, 1);
	}
	END_CATCH
}


void CExcelEx::StartExcel(void)
{
	// start excel process
	TRY
	{
		if(!m_app.CreateDispatch("Excel.Application"))
		{
			m_strMsg = "Could not start Excel.";
			//TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED, 1);
			return;
		}
		m_app.put_DisplayAlerts (VARIANT_FALSE);
		m_app.put_UserControl(FALSE);
		m_bIsExcelStarted=TRUE;
	}
	//Clean up if something went wrong.
	CATCH(CException, e)
	{
		m_strMsg = "Could not start Excel.";
		//TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED, 1);
	}
	END_CATCH
}


void CExcelEx::QuitExcel( BOOL bExit )
{
	CloseWorkBook();
	TRY
	{
		m_app.Quit();
		m_app.ReleaseDispatch();
	}
	//Clean up if something went wrong.
	CATCH(CException, e)
	{
		m_strMsg = "Could not quit Excel.";
		TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED_1, 1);
	}
	END_CATCH
	if ( bExit )
	{
		usrTerminateProcess("EXCEL.EXE");
	}
}


void CExcelEx::SelectSheet(int nNum)
{
	int nCnt;
	
	m_sheets = m_book.get_Worksheets();
	nCnt = m_sheets.get_Count();
	if(nNum<=nCnt)
	{
		m_sheet = m_sheets.get_Item(COleVariant((short)nNum));
		m_bIsSheetSelected=TRUE;
		m_sheet.Select(m_covOptional);
		m_app.put_UserControl(TRUE);
	}
	else
	{
		m_strMsg = "There is no sheet number you select.";
		TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED_1, 1);
	}
}


void CExcelEx::SelectSheet(CString strName)
{
	m_sheets = m_book.get_Worksheets();
	m_sheet = m_sheets.get_Item(COleVariant(strName));
	m_sheet.Select(m_covOptional);
	m_bIsSheetSelected=TRUE;
	m_app.put_UserControl(TRUE);
}

void CExcelEx::SetSheetName(CString strName)
{
	if(m_bIsSheetSelected)
	{
		m_sheet.put_Name(strName);
	}
	else
	{
		m_strMsg = "There is no sheet number you select.";
		TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED_1, 1);
	}
}


void CExcelEx::ShowExcel(BOOL bShow)
{
	if(m_bIsExcelStarted==TRUE)
	{
		m_app.put_Visible(bShow);
	}
	else
	{
		m_strMsg = "Excel is not started yet. Start excel first.";
		TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED_1, 1);
	}
}


void CExcelEx::Initialize(void)
{
	m_covTrue = ((short)TRUE);
	m_covFalse = ((short)FALSE);
	m_covOptional = COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	m_bIsExcelStarted=FALSE;
	m_bIsSheetSelected=FALSE;
	QuitExcel(TRUE);	//ì²˜ìŒ ?œìž‘???‘ì? ?„ë¡œê·¸ëž¨ ?´ê¸° ?¤ë¥˜ ë°©ì?
}


CString CExcelEx::ReadData(int nCol, int nRow)
{
	CString strPos;
	CString strRet= "";
	CRange range;
	if(m_bIsSheetSelected==TRUE)
	{
		strPos = GetExcelPos(nCol,nRow);
		range = m_sheet.get_Range(COleVariant(strPos), COleVariant(strPos));
		strRet = range.get_Value();
	}
	else
	{
		m_strMsg = "Sheet is not selected yet. Select sheet first.";
		TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED_1, 1);
	}
	return strRet;
}

void CExcelEx::SetPrintOut()
{
	if(m_bIsSheetSelected)
	{	// From,          To,          Copies,  Preview 
		// ActivePrinter, PrintToFile, Collate, PrToFileName 
		m_sheet.PrintOut( COleVariant ( long(1) ) , COleVariant ( long(1) ), COleVariant ( long(1) ), COleVariant ( long(0) ), 
			              COleVariant ( long(0) ), COleVariant ( long (1) ), COleVariant ( long(0) ), COleVariant ( "" ));
	}
	else
	{
		m_strMsg = "There is no sheet number you select.";
		TECmsgOut( m_Util.CStringTobstr( m_strMsg), RED_1, 1);
	}
}

