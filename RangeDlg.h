// rangedlg.h : interface of the CRangeDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CRangeDlg : public CDialogImpl<CRangeDlg>,
                 public CWinDataExchange<CRangeDlg>
{
public:
	enum { IDD = IDD_RANGE_SET };

	BEGIN_MSG_MAP(CRangeDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

  BEGIN_DDX_MAP(CRangeDlg)
    //DDX_CONTROL(IDC_RANGE_MIN, m_wndEdit)
    DDX_FLOAT(IDC_RANGE_MIN, m_minval)
    DDX_FLOAT(IDC_RANGE_MAX, m_maxval)
//    DDX_CONTROL(IDC_EDIT, m_wndEdit)
//    DDX_TEXT(IDC_EDIT, m_sEditContents)
//    DDX_INT(IDC_EDIT, m_nEditNumber)
//    DDX_CHECK(IDC_SHOW_MSG, m_bShowMsg)
  END_DDX_MAP()
  
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

  double m_minval, m_maxval;

protected:
//  CEditImpl        m_wndEdit;
  // DDX variables
//  CString m_sEditContents;
//  int m_nEditNumber;
//  bool m_bShowMsg;
};
