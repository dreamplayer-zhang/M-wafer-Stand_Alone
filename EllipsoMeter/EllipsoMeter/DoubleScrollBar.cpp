// CDoubleScrollBar - Written By David Hayes
// FREEWARE  - but please mention me in the small print!
//
// Usage: Treat this class as you would treat a CScrollBar Class, but use
// double values instead of int.
// Caution: Using SetScrollInfo & GetScrollInfo return SCROLLINFO structs
// which still deal with int values. To overcome this, I have included three
// extra methods, for the commonest tasks used with SCROLLINFO
//
// double GetPageSize() returns the size of the page in double format
// void SetPageSize(double pageSize, bool bRedraw) sets the size of the page
// in double format, and forces a redraw if bRedraw
// double GetTrackPos() returns the position of the scrollbar's trackpos
// use this instead of GetScrollInfo, and then nTrackPos, to get the current
// position when the user is tracking the scrollbar
//
// Please e-mail bug reports and suggestions to dbh@cs.stir.ac.uk
// Thanks

#include "stdafx.h"
#include "DoubleScrollBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDoubleScrollBar

CDoubleScrollBar::CDoubleScrollBar():CScrollBar(),nPrecision(32767)
{
	// precision Set this value to a maximum of 32767. The bigger the number, the more precise the scrollbar
	dMinValue = dMaxValue = dCurrentValue = 0.0;
}

CDoubleScrollBar::~CDoubleScrollBar()
{
}


BEGIN_MESSAGE_MAP(CDoubleScrollBar, CScrollBar)
	//{{AFX_MSG_MAP(CDoubleScrollBar)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDoubleScrollBar message handlers

double CDoubleScrollBar::GetScrollPos() const
{
	return dCurrentValue;
}


double CDoubleScrollBar::SetScrollPos(double dPos, bool bRedraw)
{
	if (dMaxValue == dMinValue)	// if the range is 0, CScrollBar::SetScrollRange deals with this automatically 
	{							// so all we have to do is mimic what CScrollBar will do.
		dCurrentValue = dMinValue;
		return dCurrentValue;
	}

	double dResult = dCurrentValue;
	dCurrentValue = dPos;
	if (dPos <= dMinValue)
		dCurrentValue = dMinValue; // Emulates the nature of the normal scrollbar
	if (dPos >= dMaxValue)
		dCurrentValue = dMaxValue; // Emulates the nature of the normal scrollbar

#pragma warning(disable:4244) // Disable warning "Converting 'double' to 'int', possible loss of data"
	CScrollBar::SetScrollPos((int)((double) nPrecision * (dCurrentValue-dMinValue)/(dMaxValue-dMinValue)),bRedraw);
#pragma warning(default:4244) // Re-enable warning "Converting 'double' to 'int', possible loss of data"

	return dResult;
}


void CDoubleScrollBar::GetScrollRange(double *dMinPos, double *dMaxPos)
{
	(*dMinPos) = dMinValue;
	(*dMaxPos) = dMaxValue;
}


void CDoubleScrollBar::SetScrollRange(double dMinPos, double dMaxPos, bool bRedraw)
{
	if (dMinPos == dMaxPos && dMinPos == 0.0)	// Emulate the nature of the normal scrollbar
		CScrollBar::SetScrollRange(0,0,bRedraw);
	else
	{
		dMinValue = dMinPos;
		dMaxValue = dMaxPos;
		SetScrollPos(dCurrentValue,false);	// Changing the range changes where the pointer will lie
		CScrollBar::SetScrollRange(0,nPrecision,bRedraw);
	}
}


double CDoubleScrollBar::GetScrollLimit()
{
	if (dMinValue == dMaxValue)
		return dMinValue; // Cannot carry out calculation if they are identical
	return dMaxValue + (dMaxValue-dMinValue)/(double) nPrecision;

}

double CDoubleScrollBar::GetPageSize()
{
	SCROLLINFO tempInfo;
	CScrollBar::GetScrollInfo( &tempInfo, SIF_PAGE);
	return (tempInfo.nPage / (double) nPrecision) * (dMaxValue - dMinValue);
}

void CDoubleScrollBar::SetPageSize(double dPageSize, bool bRedraw)
{
	SCROLLINFO tempInfo;
	CScrollBar::GetScrollInfo( &tempInfo, SIF_PAGE);
	if (dMinValue != dMaxValue)

#pragma warning(disable:4244) // Disable warning "Converting 'double' to 'int', possible loss of data"
		tempInfo.nPage = dPageSize * nPrecision / (dMaxValue - dMinValue);
#pragma warning(default:4244) // Re-enable warning "Converting 'double' to 'int', possible loss of data"

	CScrollBar::SetScrollInfo( &tempInfo, bRedraw);
}

double CDoubleScrollBar::GetTrackPos()
{
	SCROLLINFO tempInfo;
	CScrollBar::GetScrollInfo( &tempInfo, SIF_TRACKPOS);
	return (double)((int)(1000.0*(dMinValue + (tempInfo.nTrackPos / (double) nPrecision) * (dMaxValue - dMinValue)))/1000.0);
}
