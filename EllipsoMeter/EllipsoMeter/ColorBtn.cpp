// MyClass.cpp : implementation file
//

#include "stdafx.h"
#include "ColorBtn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMER_DRAW   1004

/////////////////////////////////////////////////////////////////////////////
// CColorBtn

CColorBtn::CColorBtn()
{
	m_bDraw  = FALSE;

	m_nBtnColor  = 0;
	m_nTextColor = 0;
	m_nTimer     = 0;
}

CColorBtn::~CColorBtn()
{
}


BEGIN_MESSAGE_MAP(CColorBtn, CButton)
	//{{AFX_MSG_MAP(CColorBtn)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorBtn message handlers
void CColorBtn::OnTimer(UINT_PTR nIDEvent) 
{
	switch (nIDEvent)
	{
		case TIMER_DRAW:
			m_bDraw = !m_bDraw;
			if( m_bDraw )
			{
				m_fnSetBtnColor(3);
				m_fnSetTextColor(0);
			}
			else
			{
				m_fnSetBtnColor(m_nBtnColor);
			}

			InvalidateRect(NULL,FALSE);
			UpdateWindow();
		break;
	}
	
	CButton::OnTimer(nIDEvent);
}

void CColorBtn::m_fnFlashBackground(BOOL bActivate)
{
	if (!bActivate)
	{
		KillTimer(TIMER_DRAW);
		m_nTimer = 0;
		
		m_fnSetBtnColor(m_nBtnColor);
		m_fnSetTextColor(m_nTextColor);

		InvalidateRect(NULL,FALSE);
		UpdateWindow();
	}

	if (bActivate)
	{
		m_nTimer = (int)SetTimer(TIMER_DRAW, 500, NULL);
	}

	return;
}

void CColorBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);		//Get device context object
	CRect rt;
	rt = lpDrawItemStruct->rcItem;		//Get button rect

	dc.FillSolidRect(rt, m_BtnColorRef);		//Fill button with USER Color

	UINT state = lpDrawItemStruct->itemState;	//Get state of the button
	if ( (state & ODS_SELECTED) )		// If it is pressed
	{
		dc.DrawEdge(rt,EDGE_SUNKEN,BF_RECT);    // Draw a sunken face
	}
	else
	{
		dc.DrawEdge(rt,EDGE_RAISED,BF_RECT);	// Draw a raised face
	}
	dc.SetTextColor(m_TextColorRef);		// Set the color of the caption to be USER Color
	CString strTemp;
	CString strData;
	GetWindowText(strTemp);		// Get the caption which have been set

	int nFind = strTemp.Find('\n');
	if( nFind == -1 )
	{
		// dc.DrawText(strTemp,rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);	// Draw out the caption
		dc.DrawText(strTemp,rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);		// Draw out the caption
	}
	else
	{
		rt.top = rt.Height()/2 - 15;
		// dc.DrawText(strTemp,rt,DT_CENTER|DT_VCENTER);// Draw out the caption
		dc.DrawText(strTemp,rt,DT_LEFT|DT_VCENTER);		// Draw out the caption
	}
	if ( (state & ODS_FOCUS ) )       // If the button is focused
	{
		// Draw a focus rect which indicates the user 
		// that the button is focused
		int iChange = 3;
		rt.top += iChange;
		rt.left += iChange;
		rt.right -= iChange;
		rt.bottom -= iChange;
		dc.DrawFocusRect(rt);
	}
	dc.Detach();
}


void CColorBtn::m_fnSetParam(int nBtnColor, int nTextColor) 
{
	m_nBtnColor  = nBtnColor;
	m_nTextColor = nTextColor;

	m_fnSetBtnColor(nBtnColor);
	m_fnSetTextColor(nTextColor);

	Invalidate(FALSE);
}

void CColorBtn::m_fnSetBtnColor(int nBtnColor)
{
	switch(nBtnColor)
	{
	case 0:
		m_BtnColorRef	= COLORDLG_0;
		break;
	case 1:
		m_BtnColorRef	= COLORDLG_1;
		break;
	case 2:
		m_BtnColorRef	= COLORDLG_2;
		break;
	case 3:
		m_BtnColorRef	= COLORDLG_3;
		break;
	case 4:
		m_BtnColorRef	= COLORDLG_4;
		break;
	case 5:
		m_BtnColorRef	= COLORDLG_5;
		break;
	case 6:
		m_BtnColorRef	= COLORDLG_6;
		break;
	case 7:
		m_BtnColorRef	= COLORDLG_7;
		break;
	case 8:
		m_BtnColorRef	= COLORDLG_8;
		break;
	case 9:
		m_BtnColorRef	= COLORDLG_9;
		break;
	case 10:
		m_BtnColorRef	= COLORDLG_10;
		break;
	case 11:
		m_BtnColorRef	= COLORDLG_11;
		break;
	case 12:
		m_BtnColorRef	= COLORDLG_12;
		break;
	case 13:
		m_BtnColorRef	= COLORDLG_13;
		break;
	case 14:
		m_BtnColorRef	= COLORDLG_14;
		break;
	case 15:
		m_BtnColorRef	= COLORDLG_15;
		break;
	case 16:
		m_BtnColorRef	= COLORDLG_16;
		break;
	default :
		break;
	}

}


void CColorBtn::m_fnSetTextColor(int nTextColor)
{
	switch(nTextColor)
	{
	case 0:
		m_TextColorRef	= COLORDLG_0;
		break;
	case 1:
		m_TextColorRef	= COLORDLG_1;
		break;
	case 2:
		m_TextColorRef	= COLORDLG_2;
		break;
	case 3:
		m_TextColorRef	= COLORDLG_3;
		break;
	case 4:
		m_TextColorRef	= COLORDLG_4;
		break;
	case 5:
		m_TextColorRef	= COLORDLG_5;
		break;
	case 6:
		m_TextColorRef	= COLORDLG_6;
		break;
	case 7:
		m_TextColorRef	= COLORDLG_7;
		break;
	case 8:
		m_TextColorRef	= COLORDLG_8;
		break;
	case 9:
		m_TextColorRef	= COLORDLG_9;
		break;
	case 10:
		m_TextColorRef	= COLORDLG_10;
		break;
	case 11:
		m_TextColorRef	= COLORDLG_11;
		break;
	case 12:
		m_TextColorRef	= COLORDLG_12;
		break;
	case 13:
		m_TextColorRef	= COLORDLG_13;
		break;
	case 14:
		m_TextColorRef	= COLORDLG_14;
		break;
	case 15:
		m_TextColorRef	= COLORDLG_15;
		break;
	case 16:
		m_TextColorRef	= COLORDLG_16;
		break;
	default :
		break;
	}
}
