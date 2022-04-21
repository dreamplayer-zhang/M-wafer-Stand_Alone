/****************************************************************/
/*																*/
/*  XmlDocument.cpp												*/
/*																*/
/*  Implementation of the CXmlDocument class.					*/
/*																*/
/*  Programmed by Pablo van der Meer							*/
/*  Copyright Pablo Software Solutions 2005						*/
/*	http://www.pablosoftwaresolutions.com						*/
/*																*/
/*  Last updated: May 16, 2005									*/
/*																*/
/****************************************************************/

#include "stdafx.h"
#include "XmlDocument.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/********************************************************************/
/*																	*/
/* Function name : CXmlDocument										*/
/* Description   : Constructor										*/
/*																	*/
/********************************************************************/
CXmlDocument::CXmlDocument()
{
	m_nLevel			= -1;
}


/********************************************************************/
/*																	*/
/* Function name : ~CXmlDocument									*/
/* Description   : Destructor										*/
/*																	*/
/********************************************************************/
CXmlDocument::~CXmlDocument()
{
	DeleteContents();
}


/********************************************************************/
/*																	*/
/* Function name : DeleteContents									*/
/* Description   : Initialize variables to their initial values.	*/
/*																	*/
/********************************************************************/
void CXmlDocument::DeleteContents()
{
	// clean up any previous data
	while(!m_RootElement.m_ChildElements.IsEmpty())
	{
		delete m_RootElement.m_ChildElements.RemoveHead();
	}
	m_pCurElement = &m_RootElement;
	m_pCurElement->m_pParentElement = NULL;
	m_RootElement.m_strName = "";
	m_RootElement.m_strData = "";
	m_RootElement.m_strAttributes = "";
	m_RootElement.m_strFind = "";
	m_RootElement.m_posFind = NULL;
}


/********************************************************************/
/*																	*/
/* Function name : Parse											*/
/* Description   : Parse XML data.									*/
/*																	*/
/********************************************************************/
BOOL CXmlDocument::Parse(LPCTSTR lpszString)
{
	// clean previous document data
	DeleteContents();

	// save string
	m_strXml = lpszString;

	BOOL bInsideTag = FALSE;

	CString strTag("");
	CString strData(""); 
	CString strResult("");

	for(int i=0; i<m_strXml.GetLength(); i++)
    {
        TCHAR ch = m_strXml[i];
        
		// begin of tag ?
		if(ch == '<')
        {
			strTag += ch;

			// add data to element
			m_pCurElement->m_strData = strData;
			// trim spaces
			m_pCurElement->m_strData.TrimLeft();
			m_pCurElement->m_strData.TrimRight();

			// clear data
            strData = "";
            
			// processing tag...
			bInsideTag = TRUE;
            continue;        
        }
		// end of tag ?
        if(ch == '>')
        {
            strTag += ch;

			// determine type and name of the tag
			int nType = ValidateTag(strTag, strResult);

			// clear tag
            strTag = "";

			// skip errors/comments/declaration
			if (nType == -1)
			{
				continue;
			}

			// start or start-end tag -> add new element
			if(nType == 0 || nType == 2)
			{
				// currently processing root element ?
				if (m_RootElement.m_strName.IsEmpty())
				{
					// split name and attributes
					int nPos = strResult.Find(' ');
					if (nPos != -1)
					{
						// set properties of root element
						m_RootElement.m_strName = strResult.Left(nPos);
						m_RootElement.m_strAttributes = strResult.Mid(nPos+1);
						// trim spaces
						m_RootElement.m_strAttributes.TrimLeft();
						m_RootElement.m_strAttributes.TrimRight();
					}
					else
					{
						m_RootElement.m_strName = strResult;
					}
				}
				else
				{
					// create new element
					CXmlElement *pElement = new CXmlElement;

					pElement->m_pParentElement = m_pCurElement;
					
					// split name and attributes
					int nPos = strResult.Find(' ');
					if (nPos != -1)
					{
						// set properties of current element
						pElement->m_strName = strResult.Left(nPos);
						pElement->m_strAttributes = strResult.Mid(nPos+1);
						// trim spaces
						pElement->m_strAttributes.TrimLeft();
						pElement->m_strAttributes.TrimRight();
					}
					else
					{
						pElement->m_strName = strResult;
					}
					m_pCurElement->m_ChildElements.AddTail(pElement);
					m_pCurElement = pElement;
				}
			}

			// end or start-end tag -> finished with current tag
			if(nType == 1 || nType == 2)
			{
				// go back to parent level
				if (m_pCurElement->m_pParentElement != NULL)
					m_pCurElement = m_pCurElement->m_pParentElement;
			}

			// processing data...
			bInsideTag = FALSE;
            continue;
        }
        
		if(bInsideTag)
		{
			// append character to tag
            strTag += ch;
		}
        else
        {
			// append character to data
			strData += ch;
        }
    }
	return TRUE;
}


/********************************************************************/
/*																	*/
/* Function name : ValidateTag										*/
/* Description   : Determine type and name of the tag.				*/
/*				   0 = start tag									*/
/*				   1 = end tag										*/
/*				   2 = start-end tag								*/
/*				   -1 = comments or declaration						*/
/*																	*/
/********************************************************************/
int CXmlDocument::ValidateTag(CString &strTag, CString &strResult)
{
	strResult = "";

	if (strTag.IsEmpty())
		return -1;

    TCHAR ch;
	TCHAR chPrevious = '0';
	
	int nResult = 0;
	int nCount = 0;

	// determine tag type
    while(nCount <strTag.GetLength())
    {
        // get next character
		ch = strTag[nCount++];

		// skip comments '<!' and declaration '<?'
        if ((chPrevious == '<' && ch == '!') || 
			(chPrevious == '<' && ch == '?'))
		{
            return -1;
		}
        else
		// is it an end-tag '</' ?
        if(chPrevious =='<' && ch == '/') 
        {
            nResult = 1;
        }
        else
		// is it a start-end-tag '<..../>' ?
        if(chPrevious =='/' && ch == '>') 
        {
            nResult = 2;
			// remove last character
			strResult.Delete(strResult.GetLength()-1, 1);
        }
        else 
		if(ch != '<' && ch != '>')
		{
			// add character
            strResult += ch;
        }
        chPrevious = ch;
    }
	return nResult;
}


/********************************************************************/
/*																	*/
/* Function name : GetFirstChild									*/
/* Description   : Get first child of element.						*/
/*																	*/
/********************************************************************/
CXmlElement *CXmlDocument::GetFirstChild(CXmlElement *pElement) 
{
	pElement->m_posFind = NULL;
	
	POSITION pos = pElement->m_ChildElements.GetHeadPosition();
	if (pos != NULL)
	{
		CXmlElement *pResult = (CXmlElement *)pElement->m_ChildElements.GetNext(pos);
		pElement->m_posFind = pos;
		return pResult;
	}
	return NULL;
}


/********************************************************************/
/*																	*/
/* Function name : GetNextSibling									*/
/* Description   : Get next child of specified element.				*/
/*																	*/
/********************************************************************/
CXmlElement *CXmlDocument::GetNextSibling(CXmlElement *pElement) 
{
	if (pElement->m_posFind)
		return (CXmlElement *)pElement->m_ChildElements.GetNext(pElement->m_posFind);
	else
		return NULL;
}


/********************************************************************/
/*																	*/
/* Function name : FindElement										*/
/* Description   : Find first occurence of specified tag.			*/
/*																	*/
/********************************************************************/
CXmlElement *CXmlDocument::FindElement(CXmlElement *pElement, LPCTSTR lpszName) 
{
	pElement->m_posFind = NULL;
	
	pElement->m_strFind = lpszName;
	
	POSITION pos = pElement->m_ChildElements.GetHeadPosition();
	while (pos != NULL)
	{
		CXmlElement *pResult = (CXmlElement *)pElement->m_ChildElements.GetNext(pos);
		if (pResult->m_strName.CompareNoCase(lpszName) == 0)
		{
			pElement->m_posFind = pos;
			return pResult;
		}
	}
	return NULL;
}


/********************************************************************/
/*																	*/
/* Function name : FindNextElement									*/
/* Description   : Find next occurence of specified tag				*/
/*																	*/
/********************************************************************/
CXmlElement *CXmlDocument::FindNextElement(CXmlElement *pElement) 
{
	while(pElement->m_posFind != NULL)
	{
		CXmlElement *pResult = (CXmlElement *)pElement->m_ChildElements.GetNext(pElement->m_posFind);
		if (pResult->m_strName.CompareNoCase(pElement->m_strFind) == 0)
		{
			return pResult;
		}
	}
	return NULL;
}


/********************************************************************/
/*																	*/
/* Function name : AddElement										*/
/* Description   : Add new element									*/
/*																	*/
/********************************************************************/
CXmlElement *CXmlDocument::AddElement(CXmlElement *pElement, LPCTSTR lpszName, LPCTSTR lpszData, LPCTSTR lpszAttributes) 
{
	CXmlElement *pNewElement = new CXmlElement;

	pNewElement->m_strName = lpszName;
	pNewElement->m_strName.TrimLeft();
	pNewElement->m_strName.TrimRight();

	if (lpszData)
	{
		pNewElement->m_strData = lpszData;
		pNewElement->m_strData.Replace("&", "&amp;");
		pNewElement->m_strData.Replace("<", "&lt;");
		pNewElement->m_strData.Replace(">", "&gt;");
	}
	if (lpszAttributes)
		pNewElement->m_strAttributes = lpszAttributes;
	
	pElement->m_ChildElements.AddTail(pNewElement);

	return pNewElement;
}


/********************************************************************/
/*																	*/
/* Function name : Generate											*/
/* Description   : Generate a XML string from elements				*/
/*																	*/
/********************************************************************/
CString CXmlDocument::Generate()
{
	CString strResult("");

	strResult = "<? XML Version=\"1.0\"?>\r\n";

	CString strTag("");

	m_nLevel = -1;
	CreateTag(&m_RootElement, strTag);

	strResult += XML_START;
	strResult += strTag;
	strResult += XML_END;

	return strResult;
}

/********************************************************************/
/*																	*/
/* Function name : Generate	Log										*/
/* Description   : Generate a XML string from elements				*/
/*																	*/
/********************************************************************/
CString CXmlDocument::GenerateLog()
{
	CString strTag("");
	CString strResult("");

	m_nLevel = -1;
	CreateTag(&m_RootElement, strTag);
	strResult  = "\r\n";
	strResult += strTag;

	return strResult;
}


/********************************************************************/
/*																	*/
/* Function name : CreateTag										*/
/* Description   : Create tag and tags from all child elements		*/
/*																	*/
/********************************************************************/
BOOL CXmlDocument::CreateTag(CXmlElement *pElement, CString &strResult)
{
	int i;

	m_nLevel++;

	// make sure we start empty
	strResult = "";

	// add spaces before start-tag
	for (i=0; i<m_nLevel; i++)
		strResult += "   ";

	// add start-tag
	strResult += "<";
	strResult += pElement->m_strName;

	if (!pElement->m_strAttributes.IsEmpty())
	{
		strResult += " ";
		strResult += pElement->m_strAttributes;
	}
	
	strResult += ">";

	if (!pElement->m_strData.IsEmpty())
	{
		strResult += pElement->m_strData;
	}
	else
	{
		strResult += "\r\n";
	}

	// process child elements
	POSITION pos = pElement->m_ChildElements.GetHeadPosition();
	while (pos != NULL)
	{
		CXmlElement *pChildElement = (CXmlElement *)pElement->m_ChildElements.GetNext(pos);

		CString strTag;
		CreateTag(pChildElement, strTag);
		strResult += strTag;
	}
	
	if (pElement->m_strData.IsEmpty())
	{
		// add spaces before end tag
		for (i=0; i<m_nLevel; i++)
			strResult += "   ";
	}

	// add end-tag
	strResult += "</";
	strResult += pElement->m_strName;
	strResult += ">\r\n";

	m_nLevel--;
	return TRUE;
}

/********************************************************************/
/*																	*/
/* Function name : CreateTagToString										*/
/* Description   : Create tag and tags from all child elements		*/
/*																	*/
/********************************************************************/
BOOL CXmlDocument::CreateTagToString(CXmlElement *pElement, CString &strResult)
{
	int i = 0;

	m_nLevel++;

	// make sure we start empty
	strResult = "";

	// add start-tag
	strResult += "<";
	strResult += pElement->m_strName;

	if (!pElement->m_strAttributes.IsEmpty())
	{
		strResult += " ";
		strResult += pElement->m_strAttributes;
	}

	strResult += ">";

	if (!pElement->m_strData.IsEmpty())
	{
		strResult += pElement->m_strData;
	}

	// process child elements
	POSITION pos = pElement->m_ChildElements.GetHeadPosition();
	while (pos != NULL)
	{
		CXmlElement *pChildElement = (CXmlElement *)pElement->m_ChildElements.GetNext(pos);

		CString strTag;
		CreateTagToString(pChildElement, strTag);
		strResult += strTag;
	}

	// add end-tag
	strResult += "</";
	strResult += pElement->m_strName;
	strResult += ">";

	m_nLevel--;
	return TRUE;
}


/********************************************************************/
/*																	*/
/* Function name : Load												*/
/* Description   : Load document from file							*/
/*																	*/
/********************************************************************/
BOOL CXmlDocument::Load(LPCTSTR lpszFileName)
{
	CWaitCursor waitCursor;

	CString strXML("");

	try
	{
		CFile inputFile(lpszFileName, CFile::modeRead);
	
		DWORD dwLength = (DWORD)inputFile.GetLength();

		inputFile.Read(strXML.GetBuffer(dwLength), dwLength);
		strXML.ReleaseBuffer();

		inputFile.Close();
	}
	catch(CFileException *ex)
	{
		ex->Delete();
		return FALSE;
	}

	// remove endofline and tabs
	strXML.Remove('\n');
	strXML.Remove('\r');
	strXML.Remove('\t');

	return Parse(strXML);
}


/********************************************************************/
/*																	*/
/* Function name : Store											*/
/* Description   : Save document to file							*/
/*																	*/
/********************************************************************/
BOOL CXmlDocument::Store(LPCTSTR lpszFileName)
{
	CWaitCursor waitCursor;

	CString strXML = Generate();

	try
	{
		CFile outputFile(lpszFileName, CFile::modeCreate | CFile::modeWrite);
		outputFile.Write(strXML, strXML.GetLength());
		outputFile.Close();
	}
	catch(CFileException *ex)
	{
		ex->Delete();
		return FALSE;
	}
	return TRUE;
}

/********************************************************************/
/*																	*/
/* Function name : DataSet											*/
/* Description   : 해당 Item에 데이터를 설정						*/
/*																	*/
/********************************************************************/
BOOL CXmlDocument::DataSet( CXmlElement *pElement, CString strstrITEM, CString strDATA )
{
	if ( pElement == NULL ) FALSE;

	CXmlElement *pChildElement = FindElement( pElement, strstrITEM );
	if ( pChildElement )
	{
		pChildElement->m_strData = strDATA;
		return TRUE;
	}
	return FALSE;
}

/********************************************************************/
/*																	*/
/* Function name : AttributeSet										*/
/* Description   : 속성을 변경한다. 가변구조체 갯수설정				*/
/*																	*/
/********************************************************************/
BOOL CXmlDocument::AttributeSet( CXmlElement *pElement, CString strITEM, CString strDATA )
{
	if ( pElement == NULL ) FALSE;

	CXmlElement *pChildElement = FindElement( pElement, strITEM );
	if ( pChildElement )
	{
		CString strEdit("");
		strEdit.Format( SUBITEM, strDATA );
		pChildElement->m_strAttributes = strEdit;
		return TRUE;
	}
	return FALSE;
}

/********************************************************************/
/*																	*/
/* Function name : FindElement										*/
/* Description   : StringArray에 설정된 Depth만큼 이동하여 포인트	*/
/*				   취득												*/
/********************************************************************/
CXmlElement* CXmlDocument::FindElement( CXmlElement *pElement, CStringArray& strLst )
{
	CString strData("");
	CXmlElement* pResult = NULL;
	for ( int i = 0; i < strLst.GetCount(); i++ )
	{
 	    strData = strLst.GetAt( i ) ;
		pResult = FindElement( pElement, strData );
		if ( pResult == NULL ) return pResult;
		pElement = pResult;
	}
	return pResult;
}

/********************************************************************/
/*																	*/
/* Function name : FindData											*/
/* Description   : 해당 ITEM의 값을 취득							*/
/*				   													*/
/********************************************************************/
BOOL CXmlDocument::FindData( CXmlElement *pElement, CString strstrITEM, CString& strDATA )
{
	if ( pElement == NULL ) FALSE;
	CXmlElement *pChildElement = FindElement( pElement, strstrITEM );
	if ( pChildElement )
	{
		strDATA = pChildElement->m_strData;
		return true;
	}
	strDATA = _T("");
	return false;
}

/********************************************************************/
/*																	*/
/* Function name : RemoveElement									*/
/* Description   : 가변장 구조체를 삭제한다							*/
/*				   단 서브 구조체가 1개이상일경우에는 전체 삭제하고	*/
/*				   ITEM을 추가하는 방식으로 진행해야 한다	        */
/********************************************************************/
BOOL CXmlDocument::RemoveElement( CXmlElement *pElement, CStringArray& strLst )
{
	if ( pElement == NULL ) FALSE;

	CXmlElement* pResult = NULL;
	CString strData("");
	int nLstCnt          = 0;
	nLstCnt              = strLst.GetCount();

	if ( nLstCnt < 2 ) return false;

	for ( int i = 0; i < nLstCnt; i++ )
	{
		strData = strLst.GetAt( i ) ;
		pResult = FindElement( pElement, strData );
		if ( pResult == NULL ) return false;
		pElement = pResult;
	}
	if ( pResult == NULL ) return false;

	while(!pElement->m_ChildElements.IsEmpty())
	{
		delete pResult->m_ChildElements.RemoveHead();
	}
	return true;
}

bool CXmlDocument::AnsiToUtf8(const char * pszAnsi, std::string & strOutput/*CString & strOutput*/)
{
	wchar_t uni[MAX_BUFF_SIZE+1];
	ZeroMemory(uni, MAX_BUFF_SIZE+1);

	char*   pszUtf8 = NULL;
	int     iLength;

	iLength = MultiByteToWideChar(CP_ACP, 0, pszAnsi, (int)strlen(pszAnsi), NULL, NULL);
	if (iLength == 0)
	{
		return false;
	}

	MultiByteToWideChar(CP_ACP, 0, pszAnsi, (int)strlen(pszAnsi), uni, iLength);

	iLength = WideCharToMultiByte(CP_UTF8, 0, uni, -1, NULL, 0, NULL, NULL);
	if (iLength == 0)
	{
		return false;
	}

	pszUtf8 = new char[iLength];
	if (pszUtf8 == NULL)
	{
		return false;
	}

	WideCharToMultiByte(CP_UTF8, 0, uni, -1, pszUtf8, iLength, NULL, NULL);
	strOutput = pszUtf8;

	delete(pszUtf8);

	return true;
}

bool CXmlDocument::Utf8ToAnsi(const char * pszCode, std::string & strOutput/*CString & strOutput*/)
{
	wchar_t uni[MAX_BUFF_SIZE+1];
	ZeroMemory(uni, MAX_BUFF_SIZE+1);

	char*   pszAnsi = NULL;
	int     iLength;

	iLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, (int)strlen(pszCode) + 1, NULL, NULL);
	if (iLength == 0)
	{
		return false;
	}

	// Change UTF-8 to Unicode (UTF-16)  
	MultiByteToWideChar(CP_UTF8, 0, pszCode, (int)strlen(pszCode), uni, iLength);

	iLength = WideCharToMultiByte(CP_ACP, 0, uni, -1, NULL, 0, NULL, NULL);
	if (iLength == 0)
	{
		return false;
	}

	pszAnsi = new char[iLength];
	if (pszAnsi == NULL)
	{
		return false;
	}

	WideCharToMultiByte(CP_ACP, 0, uni, -1, pszAnsi, iLength, NULL, NULL);
	strOutput = pszAnsi;

	delete(pszAnsi);

	return true;
}

CXmlElement * CXmlDocument::GetFindSubElement( CString strSubElement ) 
{
	CXmlElement *pElement = GetRootElement();

	CXmlElement *pItemElement = FindElement( pElement, strSubElement );
	if ( pItemElement )
	{
		return pItemElement;
	}
	return NULL;
}

BOOL CXmlDocument::GetTagToString( CString strSubElement,
	                               CString& strResult ) 
{
	CXmlElement *pElement = GetFindSubElement( strSubElement );
	if ( pElement )
	{
		if ( CreateTagToString( pElement, strResult ) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

/********************************************************************/
/*																	*/
/* Function name : FindDataList Result								*/
/* Description   : 하부 데이터의 리스트를 읽어들인다				*/
/*																	*/
/*															        */
/********************************************************************/
BOOL CXmlDocument::FindDataList(CXmlElement *pElement,
	CStringArray& strNAME,
	CStringArray& strCol,
	CStringArray& strRow,
	CStringArray& strSub,
	CStringArray& strX,
	CStringArray& strY,
	CStringArray& strZ)
{
	if (pElement == NULL) FALSE;

	CXmlElement *pItemElement = GetFirstChild(pElement);
	while (pItemElement != NULL)
	{
		CXmlElement *pNameElement = FindElement(pItemElement, SR_NAME);
		if (pNameElement != NULL)
		{
			strNAME.Add(pNameElement->m_strData);
		}
		CXmlElement *pColElement = FindElement(pItemElement, SR_COL);
		if (pColElement != NULL)
		{
			strCol.Add(pColElement->m_strData);
		}
		CXmlElement *pRowElement = FindElement(pItemElement, SR_ROW);
		if (pRowElement != NULL)
		{
			strRow.Add(pRowElement->m_strData);
		}
		CXmlElement *pSubElement = FindElement(pItemElement, SR_SUB);
		if (pSubElement != NULL)
		{
			strSub.Add(pSubElement->m_strData);
		}
		CXmlElement *pXElement = FindElement(pItemElement, SR_X);
		if (pXElement != NULL)
		{
			strX.Add(pXElement->m_strData);
		}
		CXmlElement *pYElement = FindElement(pItemElement, SR_Y);
		if (pYElement != NULL)
		{
			strY.Add(pYElement->m_strData);
		}
		CXmlElement *pZElement = FindElement(pItemElement, SR_Z);
		if (pZElement != NULL)
		{
			strZ.Add(pZElement->m_strData);
		}
		pItemElement = GetNextSibling(pElement);
	}

	return true;
}
/********************************************************************/
/*																	*/
/* Function name : FindDataList Result								*/
/* Description   : 하부 데이터의 리스트를 읽어들인다				*/
/*																	*/
/*															        */
/********************************************************************/
BOOL CXmlDocument::FindDataList( CXmlElement *pElement,
									CStringArray& strNAME,
									CStringArray& strCol,
									CStringArray& strRow, 
									CStringArray& strColDouble,
									CStringArray& strRowDouble, 
									CStringArray& strSub,
									CStringArray& strX,
									CStringArray& strY, 
									CStringArray& strZ )
{
	if ( pElement == NULL ) FALSE;

	CXmlElement *pItemElement = GetFirstChild( pElement );
	while ( pItemElement != NULL)
	{
		CXmlElement *pNameElement = FindElement( pItemElement, SR_NAME );
		if ( pNameElement != NULL )
		{
			strNAME.Add( pNameElement->m_strData );
		}
		CXmlElement *pColElement = FindElement(pItemElement, SR_COL);
		if (pColElement != NULL)
		{
			strCol.Add(pColElement->m_strData);
		}
		CXmlElement *pRowElement = FindElement(pItemElement, SR_ROW);
		if (pRowElement != NULL)
		{
			strRow.Add(pRowElement->m_strData);
		}
		CXmlElement *pColDoubleElement = FindElement(pItemElement, SR_COL_DOUBLE);
		if (pColDoubleElement != NULL)
		{
			strColDouble.Add(pColDoubleElement->m_strData);
		}
		CXmlElement *pRowDoubleElement = FindElement(pItemElement, SR_ROW_DOUBLE);
		if (pRowDoubleElement != NULL)
		{
			strRowDouble.Add(pRowDoubleElement->m_strData);
		}
		CXmlElement *pSubElement = FindElement(pItemElement, SR_SUB);
		if (pSubElement != NULL)
		{
			strSub.Add(pSubElement->m_strData);
		}
		CXmlElement *pXElement = FindElement(pItemElement, SR_X);
		if (pXElement != NULL)
		{
			strX.Add(pXElement->m_strData);
		}
		CXmlElement *pYElement = FindElement(pItemElement, SR_Y);
		if (pYElement != NULL)
		{
			strY.Add(pYElement->m_strData);
		}
		CXmlElement *pZElement = FindElement( pItemElement, SR_Z );
		if ( pZElement != NULL )
		{
			strZ.Add( pZElement->m_strData );
		}
		pItemElement = GetNextSibling( pElement );
	}

	return true;
}

/********************************************************************/
/*																	*/
/* Function name : FindDataList Result								*/
/* Description   : 하부 데이터의 리스트를 읽어들인다				*/
/*																	*/
/*															        */
/********************************************************************/
BOOL CXmlDocument::FindItemDataList(CXmlElement *pElement, int nLayer, t_ITEM_INFO &stData)
{
	if (pElement == NULL) FALSE;

	int     nLoopCnt = 0;
	CString strWork("");

	CXmlElement *pListElement = GetFirstChild(pElement);
	while (pListElement != NULL)
	{
		for (int i = 0; i < nLayer; i++)
		{
			for (int j = 0; j < MAX_ITEM; j++)
			{
				strWork.Format(SR_V, i + 1, j + 1);
				CXmlElement *pItemElement = FindElement(pListElement, strWork);
				if (pItemElement != NULL)
				{
					stData.dItem[nLoopCnt][i][j] = atof(pItemElement->m_strData);
				}
			}
		}
		nLoopCnt++;
		pListElement = GetNextSibling(pElement);
	}

	return true;
}

/********************************************************************/
/*																	*/
/* Function name : FindDataList USER INFO							*/
/* Description   : 하부 데이터의 리스트를 읽어들인다				*/
/*																	*/
/*															        */
/********************************************************************/
BOOL CXmlDocument::FindUserList( CXmlElement *pElement,
									CStringArray& strID, 
									CStringArray& strNAME, 
									CStringArray& strJOB, 
									CStringArray& strPASSWORD, 
									CStringArray& strLEVEL,
									CStringArray& strDESCRIPTION)
{
	if ( pElement == NULL ) FALSE;

	CXmlElement *pItemElement = GetFirstChild( pElement );
	while ( pItemElement != NULL)
	{
		CXmlElement *pUseElement = FindElement( pItemElement, USER_ID );
		if ( pUseElement != NULL )
		{
			strID.Add( pUseElement->m_strData );
		}
		CXmlElement *pNameElement = FindElement( pItemElement, USER_NAME );
		if ( pNameElement != NULL )
		{
			strNAME.Add( pNameElement->m_strData );
		}
		CXmlElement *pXElement = FindElement( pItemElement, USER_PASSWORD );
		if ( pXElement != NULL )
		{
			strPASSWORD.Add( pXElement->m_strData );
		}
		CXmlElement *pYElement = FindElement( pItemElement, USER_JOB );
		if ( pYElement != NULL )
		{
			strJOB.Add( pYElement->m_strData );
		}
		CXmlElement *pZElement = FindElement( pItemElement, USER_LEVEL );
		if ( pZElement != NULL )
		{
			strLEVEL.Add( pZElement->m_strData );
		}
		CXmlElement *pRElement = FindElement( pItemElement, USER_DESCRIPTION );
		if ( pRElement != NULL )
		{
			strDESCRIPTION.Add( pRElement->m_strData );
		}
		pItemElement = GetNextSibling( pElement );
	}

	return true;
}