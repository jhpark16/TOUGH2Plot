// aboutdlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "rangedlg.h"
#include "View.h"

extern PTSATRecord mPTSAT;

LRESULT CRangeDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	m_minval = mPTSAT.GetSetPlotData(0,1,0.0,false);
  m_maxval = mPTSAT.GetSetPlotData(0,2,0.0,false);
  double	minOrgval = mPTSAT.GetSetPlotData(0,3,0.0,false);
  double	maxOrgval = mPTSAT.GetSetPlotData(0,4,0.0,false);
  CString tstr;
  if(fabs(minOrgval)>0.001 && fabs(minOrgval)<1000 &&
     fabs(maxOrgval)>0.001 && fabs(maxOrgval)<1000)
    tstr.Format(_T("The range is %.3lf and %.3lf"),minOrgval,maxOrgval);
  else
    tstr.Format(_T("The range is %8.3le and %8.3le"),minOrgval,maxOrgval);
	SetDlgItemTextW(IDC_RANGE_TEXT,tstr);
  DoDataExchange(false);
	return TRUE;
}

LRESULT CRangeDlg::OnOK(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // Transfer data from the controls to member variables.
  if ( !DoDataExchange(true) )
      return 0;
  EndDialog(wID);
	return 0;
}

LRESULT CRangeDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  EndDialog(wID);
	return 0;
}

