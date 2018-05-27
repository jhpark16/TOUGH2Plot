// rangedlg.h : interface of the CRangeDlg class
//
// Author: Jungho Park
// Date: July 2018
// Dialog window for a user to set minimum and maximum values
// of the plot data
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

// Class definition for the data range selection dialog
class CRangeDlg : public CDialogImpl<CRangeDlg>,
                 public CWinDataExchange<CRangeDlg>
{
public:
  // Dialog box ID for this range set dialog box
	enum { IDD = IDD_RANGE_SET };
  // Declare satic function ProcessWindowMessage
	BEGIN_MSG_MAP(CRangeDlg)
    // Calls message handler
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    // Calls command message handler
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

  // Declare DoDataExchange function with several exchanging members
  BEGIN_DDX_MAP(CRangeDlg)
    // Minimum value for the data range
    // The exchange variables could be a float or double type
    DDX_FLOAT(IDC_RANGE_MIN, m_minval)
    // Maximum value for the data range
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
  // Prototypes of message hanlders
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  // member variables exchanged by the dialog box
  double m_minval, m_maxval;

protected:
};
