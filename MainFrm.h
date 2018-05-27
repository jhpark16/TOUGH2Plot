// MainFrm.h : interface of the CMainFrame class
//
// Author: Jungho Park
// Date: July 2018
// MainFrm.h is a header file for MainFrm.cpp. 
// This interface contains declaration for menu commands and shared variables.
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ToolBarHelper.h"

// CMainFrame hanldles menu commands.
class CMainFrame : 
	public CFrameWindowImpl<CMainFrame>, 
	public CUpdateUI<CMainFrame>,
	public CMessageFilter, public CIdleHandler,
  public CToolBarHelper<CMainFrame>
{
public:
  // Declare a static member (GetWndClassInfo())
  // GetWndClassInfo returns window class 
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)
  // CView variable from CDocument/CView architecture
	CView m_view;
  // member vairable for Command Bar Control
	CCommandBarCtrl m_CmdBar;
  // The command bar has three combo box controls for Time, Plot data,
  // and plot type
  CComboBox m_wndComboTime, m_wndComboPlot, m_wndComboPlotType;
  // The container variable for the status bar at the bottom
  CMultiPaneStatusBarCtrl m_wndStatusBar;

  // Pretranslate Windows meesages such as accelerator keys
  // before they reach the main application
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
    // Process Accelerator keys for menus
		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;
    // Additional opportunity to pretranslate the message by CView
		return m_view.PreTranslateMessage(pMsg);
	}

  // When the application is idle, 
	virtual BOOL OnIdle()
	{
    // update the status of the toolbar.
    UIUpdateToolBar();
		return FALSE;
	}
  // Declrare static function GetUpdateUIMap() 
	BEGIN_UPDATE_UI_MAP(CMainFrame)
    // Declare static data for the type of the ui element
    // for Toolbar and status bar
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

  // Declare satic function ProcessWindowMessage
	BEGIN_MSG_MAP(CMainFrame)
    // Declare the type and function of the WM_CREATE Windows message
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
    MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover)
    MESSAGE_HANDLER(WM_SIZE, OnSize)
    COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
    COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
    COMMAND_ID_HANDLER(ID_FILE_REOPEN, OnFileReOpen)
    COMMAND_ID_HANDLER(ID_FILE_SAVEMOVIE, OnFileSaveMovie)
    COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
    COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
    CHAIN_MSG_MAP(CToolBarHelper<CMainFrame>)
  END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

  // Declare the prototype of variaous message handlers
  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
  LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnMouseHover(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnFileReOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnFileSaveMovie(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  // OnToolBarCombo is called when the selection is changed
  void OnToolBarCombo(HWND hWndCombo, UINT nID, int nSel, LPCTSTR lpszText, DWORD dwItemData);

protected:
  // Custom function to open the plot data file
  void CMainFrame::OpenPlot(CString sSelectedFile);

};
// Get image encoder clsid
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
