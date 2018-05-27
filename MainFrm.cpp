// MainFrm.cpp : implmentation of the CMainFrame class
//
// Author: Jungho Park
// Date: July 2018
// MainFrm.cpp handles the commands from the main menu
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "View.h"
#include "MainFrm.h"

PTSATRecord mPTSAT;

// OnCreate is called when the application is created
LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  //unsigned int old_exponent_format = _set_output_format(_TWO_DIGIT_EXPONENT);
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar(images)
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	// Create a combobox that doesn't need to be updated
	m_wndComboTime = CreateToolbarComboBox(hWndToolBar, ID_COMBO_PLACEHOLDER, 7, 8);
	m_wndComboPlot = CreateToolbarComboBox(hWndToolBar, ID_COMBO_PLACEHOLDER2, 12, 8);
	m_wndComboPlot.AddString(_T("SOIL"));	
	m_wndComboPlot.AddString(_T("SL"));
	m_wndComboPlot.AddString(_T("SG"));
	m_wndComboPlot.AddString(_T("TEMPERATURE"));
	m_wndComboPlot.AddString(_T("PRESSURE"));
	m_wndComboPlot.AddString(_T("PCAP"));
	m_wndComboPlot.AddString(_T("P+PCAP"));
	m_wndComboPlot.AddString(_T("log K"));
	m_wndComboPlot.SetCurSel(0);
	m_wndComboPlotType = CreateToolbarComboBox(hWndToolBar, ID_COMBO_PLACEHOLDER3, 8, 8);
	m_wndComboPlotType.AddString(_T("Area View"));
	m_wndComboPlotType.AddString(_T("Velocity Field"));
	m_wndComboPlotType.AddString(_T("Time Trend"));
	m_wndComboPlotType.SetCurSel(0);

  m_hWndStatusBar = m_wndStatusBar.Create ( *this );
  UIAddStatusBar ( m_hWndStatusBar );

	//CreateSimpleStatusBar();
	int anPanes[] = { ID_DEFAULT_PANE, ID_PANE_VALUE};
  m_wndStatusBar.SetPanes ( anPanes, 2, false );
  m_wndStatusBar.SetPaneWidth(ID_PANE_VALUE,400);
  m_wndStatusBar.SetPaneText(ID_PANE_VALUE,_T(""));

	m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	m_view.SetChainEntry(1521, this);

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);
	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: add code to initialize document
	return 0;
}

int FetchARecord(int fh1, char *buffer, int *recLen)
{
  unsigned int bytesread;
#ifdef M64
  if( (bytesread = _read( fh1, buffer, 4)) <= 0)
  { return bytesread;/*error */ }
  else
  {
    *recLen = *((int*)buffer);
    bytesread = _read(fh1, buffer, (*recLen+4));
    if(bytesread<=0)
    {  /*error */ }
    return bytesread;
  }
#else
  bytesread = _read(fh1, buffer, (*recLen));
  return bytesread;
#endif
}

int ParseDelimiters(wchar_t* pStr, wchar_t* pDelimeters, wchar_t *pwch[],int maxN)
{
  wchar_t *pwstr;
  int i, n;
  size_t sLen;
  sLen = wcslen(pStr);
  pwch[0] = pStr;
  for(i=0;i<maxN-1;i++)
  {
    if(pwch[i]>=pStr+sLen) return i;
    n = wcscspn(pwch[i],pDelimeters);
    pwch[i][n] = _T('\0');
    pwch[i+1] = pwch[i]+n+1;
  }
  return i;
}

void CMainFrame::OpenPlot(CString sSelectedFile)
{
  int fh1, fh2, idx;
  T2PTSATHead *pPTHead;
  T2PTSATRecord *pPTRecord;
  T2FluxHead *pFluxHead;
  T2FluxRecord *pFluxRecord;

  FILE *fp1;
#define MAX_STR 1000  
  char str[MAX_STR+1], tstr[MAX_STR+1], *pStr[20];
  int paramCount;
  int iLoc = sSelectedFile.ReverseFind(_T('\\'));
  sSelectedFile = sSelectedFile.Left(iLoc+1);
  sSelectedFile += _T("HGRID.TPG");
  if((fp1 = _wfopen(sSelectedFile,_T("rt")))==NULL)
  { MessageBox(_T("Can't open HGRID.TPG")); }
  else
  {
    // Read Grid
    fgets(str,MAX_STR,fp1);
    //wcscpy(tstr,wstr);
    sscanf(str,"%lf,%lf,%lf,%lf,%lf,%lf,%d,%d\n",&(mPTSAT.BoundBox.left),
      &(mPTSAT.BoundBox.right),&(mPTSAT.BoundBox.top),&(mPTSAT.BoundBox.bottom),
      &mPTSAT.DX,&mPTSAT.DY,&mPTSAT.nColumns,&mPTSAT.nRows);
    idx=0;
    mPTSAT.xscale = PAINT_DIM/(mPTSAT.BoundBox.right-mPTSAT.BoundBox.left);
    mPTSAT.yscale = PAINT_DIM/4*3/(mPTSAT.BoundBox.bottom-mPTSAT.BoundBox.top);
    if(mPTSAT.xscale>mPTSAT.yscale)
    { mPTSAT.xscale=mPTSAT.yscale; }//mPTSAT.yscale=-mPTSAT.yscale; 
    else
    { mPTSAT.yscale=mPTSAT.xscale; }//mPTSAT.yscale=-mPTSAT.xscale;
    mPTSAT.ln_K_Avg = 0;
    while(fgets(str,MAX_STR,fp1)!=NULL)
    {
      //wcscpy(tstr,wstr);
      double tmp,tmp2,tmp3;
      strncpy(mPTSAT.ELEM[idx],str,5); mPTSAT.ELEM[idx][5]='\0';
      sscanf(str+6,"%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf",&(mPTSAT.INDEX[idx]),&(mPTSAT.ln_K[idx]),&(mPTSAT.Z_top[idx]),&(mPTSAT.Z_bottom[idx]),
        &(mPTSAT.boxD[idx].left),&(mPTSAT.boxD[idx].right),&(mPTSAT.boxD[idx].top),&(mPTSAT.boxD[idx].bottom));
      mPTSAT.box[idx].left = mPTSAT.boxD[idx].left*mPTSAT.xscale;
      mPTSAT.box[idx].right = mPTSAT.boxD[idx].right*mPTSAT.xscale;
      mPTSAT.box[idx].top = mPTSAT.boxD[idx].top*mPTSAT.yscale;
      mPTSAT.box[idx].bottom = mPTSAT.boxD[idx].bottom*mPTSAT.yscale;
      mPTSAT.X[idx] = (mPTSAT.box[idx].left+mPTSAT.box[idx].right)/2.0;
      mPTSAT.Y[idx] = (mPTSAT.box[idx].top+mPTSAT.box[idx].bottom)/2.0;
      mPTSAT.Z[idx] = (mPTSAT.Z_bottom[idx]+mPTSAT.Z_top[idx])/2.0;
      mPTSAT.ln_K[idx] /=log(10.0);
      tmp = mPTSAT.ln_K[idx];
      if(tmp<mPTSAT.ln_K_RANGE[0] && _strnicmp(mPTSAT.ELEM[idx],"ZZZ",3)!=0) 
        mPTSAT.ln_K_RANGE[0]=tmp;
      if(tmp>mPTSAT.ln_K_RANGE[1] && _strnicmp(mPTSAT.ELEM[idx],"ZZZ",3)!=0) 
        mPTSAT.ln_K_RANGE[1]=tmp;
      mPTSAT.ln_K_Avg += tmp;
      idx++;
      //paramCount = ParseDelimiters(tstr,_T(",; \t\n\"\'()[]"),pStr,20);
    }
    mPTSAT.nElements = idx;
    if(mPTSAT.nColumns==0)
    {
      mPTSAT.nColumns = mPTSAT.nRows = sqrt((double)mPTSAT.nElements);
    }
    if(fabs(mPTSAT.DX)<1e-10)
    {
      mPTSAT.DX = mPTSAT.DY = mPTSAT.BoundBox.bottom/mPTSAT.nRows;
    }
    mPTSAT.ln_K_RANGE[2] = mPTSAT.ln_K_RANGE[0];
    mPTSAT.ln_K_RANGE[3] = mPTSAT.ln_K_RANGE[1];
    mPTSAT.ln_K_Avg /= mPTSAT.nElements-3;
    mPTSAT.bClientUpdated = false;
    fclose(fp1);
    
    int iLoc = sSelectedFile.ReverseFind(_T('\\'));
    sSelectedFile = sSelectedFile.Left(iLoc+1);
    sSelectedFile += _T("PTSAT.TPL");
    fh1 = _wopen(sSelectedFile,_O_BINARY|_O_RDONLY,_S_IREAD);
    if(fh1 == -1 ) MessageBox(_T("Can't open PTSAT.TPL"));
    else
    {
      // Read
      int recLen, byteread, sStart=0, sLen;
      char buffer[MAX_RECORD];
      double val,val2;
#define MAX_TITLE 70
#define LEN_NAME 13

      CString tCstr;
      mPTSAT.PRESSURE_RANGE[0] = 1e200; mPTSAT.PRESSURE_RANGE[1] = 1e-200;
      mPTSAT.TEMPERATURE_RANGE[0] = 1e200; mPTSAT.TEMPERATURE_RANGE[1] = 1e-200;
      mPTSAT.SL_RANGE[0] = 1e200; mPTSAT.SL_RANGE[1] = 1e-200;
      mPTSAT.SG_RANGE[0] = 1e200; mPTSAT.SG_RANGE[1] = 1e-200;
      mPTSAT.SOIL_RANGE[0] = 1e200; mPTSAT.SOIL_RANGE[1] = 1e-200;
      mPTSAT.PCAP_RANGE[0] = 1e200; mPTSAT.PCAP_RANGE[1] = 1e-200;
      mPTSAT.P_REAL_RANGE[0] = 1e200; mPTSAT.P_REAL_RANGE[1] = 1e-200;
      mPTSAT.nTimes = 0;
      m_wndComboTime.ResetContent();
      tCstr = _T("TOUGH2PLOT - ");
      sLen = sSelectedFile.GetLength();
      if(sLen>MAX_TITLE) {sStart=sLen-MAX_TITLE; sLen=MAX_TITLE; }
      tCstr += sSelectedFile.Mid(sStart);
      SetWindowTextW(tCstr);
#ifdef M64
      recLen = 0x68;
#else
      recLen = 0x34;
#endif
      while(FetchARecord(fh1,buffer,&recLen)>0)
      {
        pPTHead = (T2PTSATHead *)buffer;
        int i, idx = mPTSAT.nTimes;
        mPTSAT.Time[idx] = pPTHead->SUMTIM;

        mPTSAT.Cycle[idx] = pPTHead->KCYC;
        if(mPTSAT.nElements != pPTHead->NEL) 
        {
          MessageBox(_T("# of elements are different"));
          break;
        }
        if(mPTSAT.nElements>=MAX_ELEM) { MessageBox(_T("Number of elements exceeded the max elements")); }

        mPTSAT.PRESSURE_Avg[idx] = 0.0;
        mPTSAT.TEMPERATURE_Avg[idx] = 0.0;
        mPTSAT.SG_Avg[idx] = 0.0;
        mPTSAT.SL_Avg[idx] = 0.0;
        mPTSAT.SOIL_Avg[idx] = 0.0;
        mPTSAT.PCAP_Avg[idx] = 0.0;
        for(i=0;i<mPTSAT.nElements;i++)
        {
#ifdef M64
          recLen = 0x45;
#else
          recLen = 0x25;
#endif
          byteread = FetchARecord(fh1,buffer,&recLen);
          if(byteread<=0) break;
          pPTRecord = (T2PTSATRecord *)buffer;
          if(idx==0)
          {
            memcpy(mPTSAT.ELEM[i],pPTRecord->ELEM,5);
            mPTSAT.ELEM[i][5]='\0';
            mPTSAT.INDEX[i] = pPTRecord->INDEX;
          }
          val = pPTRecord->PRESSURE;
          if(val<mPTSAT.PRESSURE_RANGE[0]) mPTSAT.PRESSURE_RANGE[0]=val;
          if(val>mPTSAT.PRESSURE_RANGE[1]) mPTSAT.PRESSURE_RANGE[1]=val;
          mPTSAT.PRESSURE[idx][i] = val;
          val2 = val;
          if(i<mPTSAT.nElements-3) mPTSAT.PRESSURE_Avg[idx] += val;
          val = pPTRecord->TEMPERATURE;
          if(val<mPTSAT.TEMPERATURE_RANGE[0]) mPTSAT.TEMPERATURE_RANGE[0]=val;
          if(val>mPTSAT.TEMPERATURE_RANGE[1]) mPTSAT.TEMPERATURE_RANGE[1]=val;
          mPTSAT.TEMPERATURE[idx][i] = val;
          if(i<mPTSAT.nElements-3) mPTSAT.TEMPERATURE_Avg[idx] += val;
          val = pPTRecord->SG;
          if(val<mPTSAT.SG_RANGE[0]) mPTSAT.SG_RANGE[0]=val;
          if(val>mPTSAT.SG_RANGE[1]) mPTSAT.SG_RANGE[1]=val;
          mPTSAT.SG[idx][i] = val;
          if(i<mPTSAT.nElements-3) mPTSAT.SG_Avg[idx] += val;
          val = pPTRecord->SL;
          if(val<mPTSAT.SL_RANGE[0]) mPTSAT.SL_RANGE[0]=val;
          if(val>mPTSAT.SL_RANGE[1]) mPTSAT.SL_RANGE[1]=val;
          mPTSAT.SL[idx][i] = val;
          if(i<mPTSAT.nElements-3) mPTSAT.SL_Avg[idx] += val;
          val = pPTRecord->SOIL;
          if(val<mPTSAT.SOIL_RANGE[0]) mPTSAT.SOIL_RANGE[0]=val;
          if(val>mPTSAT.SOIL_RANGE[1]) mPTSAT.SOIL_RANGE[1]=val;
          mPTSAT.SOIL[idx][i] = val;
          if(i<mPTSAT.nElements-3) mPTSAT.SOIL_Avg[idx] += val;
          val = pPTRecord->PCAP;
          if(val<mPTSAT.PCAP_RANGE[0]) mPTSAT.PCAP_RANGE[0]=val;
          if(val>mPTSAT.PCAP_RANGE[1]) mPTSAT.PCAP_RANGE[1]=val;
          mPTSAT.PCAP[idx][i] = val;
          if(i<mPTSAT.nElements-3) mPTSAT.PCAP_Avg[idx] += val;
          val += val2;
          if(val<mPTSAT.P_REAL_RANGE[0]) mPTSAT.P_REAL_RANGE[0]=val;
          if(val>mPTSAT.P_REAL_RANGE[1]) mPTSAT.P_REAL_RANGE[1]=val;
          if(i<mPTSAT.nElements-3) mPTSAT.P_REAL_Avg[idx] += val;
        }
        mPTSAT.PRESSURE_RANGE[2] = mPTSAT.PRESSURE_RANGE[0];
        mPTSAT.PRESSURE_RANGE[3] = mPTSAT.PRESSURE_RANGE[1];
        mPTSAT.PRESSURE_Avg[idx] /= mPTSAT.nElements-3;
        mPTSAT.TEMPERATURE_RANGE[2] = mPTSAT.TEMPERATURE_RANGE[0];
        mPTSAT.TEMPERATURE_RANGE[3] = mPTSAT.TEMPERATURE_RANGE[1];
        mPTSAT.TEMPERATURE_Avg[idx] /= mPTSAT.nElements-3;
        mPTSAT.SG_RANGE[2] = mPTSAT.SG_RANGE[0];
        mPTSAT.SG_RANGE[3] = mPTSAT.SG_RANGE[1];
        mPTSAT.SG_Avg[idx] /= mPTSAT.nElements-3;
        mPTSAT.SL_RANGE[2] = mPTSAT.SL_RANGE[0];
        mPTSAT.SL_RANGE[3] = mPTSAT.SL_RANGE[1];
        mPTSAT.SL_Avg[idx] /= mPTSAT.nElements-3;
        mPTSAT.SOIL_RANGE[2] = mPTSAT.SOIL_RANGE[0];
        mPTSAT.SOIL_RANGE[3] = mPTSAT.SOIL_RANGE[1];
        mPTSAT.SOIL_Avg[idx] /= mPTSAT.nElements-3;
        mPTSAT.PCAP_RANGE[2] = mPTSAT.PCAP_RANGE[0];
        mPTSAT.PCAP_RANGE[3] = mPTSAT.PCAP_RANGE[1];
        mPTSAT.PCAP_Avg[idx] /= mPTSAT.nElements-3;
        mPTSAT.P_REAL_RANGE[2] = mPTSAT.P_REAL_RANGE[0];
        mPTSAT.P_REAL_RANGE[3] = mPTSAT.P_REAL_RANGE[1];
        mPTSAT.P_REAL_Avg[idx] /= mPTSAT.nElements-3;
        if(i==mPTSAT.nElements)
        {
          tCstr.Format(_T("%9.0lf"),mPTSAT.Time[idx]);
          m_wndComboTime.AddString(tCstr);
          mPTSAT.nTimes++;
        }
        if(mPTSAT.nTimes>=MAX_TIME) { MessageBox(_T("Number of time level exceeded the max time")); }
#ifdef M64
        recLen = 0x68;
#else
        recLen = 0x34;
#endif
      };
      mPTSAT.indexTime = mPTSAT.nTimes-1;
      m_wndComboTime.SetCurSel(mPTSAT.indexTime);
      _close( fh1 );

      tCstr = sSelectedFile;
      tCstr.Replace(_T("PTSAT.TPL"),_T("FLUX.TPL"));
      fh1 = _wopen(tCstr,_O_BINARY|_O_RDONLY,_S_IREAD);
      if(fh1 == -1 ) MessageBox(_T("Can't open FLUX.TPL"));
      else
      {
        int idxT;
        idxT = 0;
        for(int j=0;j<mPTSAT.nTimes;j++)
        {
          for(int i=0;i<mPTSAT.nElements;i++)
          {
            mPTSAT.X_VEL_AQ[j][i] = 0;
            mPTSAT.Y_VEL_AQ[j][i] = 0;
            mPTSAT.X_VEL_OIL[j][i] = 0;
            mPTSAT.Y_VEL_OIL[j][i] = 0;
          }
        }
#ifdef M64
      recLen = 0x20;
#else
      recLen = 0x10;
#endif
        while(FetchARecord(fh1,buffer,&recLen)>0)
        {
          int i,idx1,idx2;
          pFluxHead = (T2FluxHead *)buffer;
          mPTSAT.nConnection = pFluxHead->NCON;
          for(i=0;i<mPTSAT.nConnection;i++)
          {
#ifdef M64
            recLen = 0x52;
#else
            recLen = 0x2e;
#endif
            byteread = FetchARecord(fh1,buffer,&recLen);
            if(byteread<=0) break;
            pFluxRecord = (T2FluxRecord *)buffer;
            idx1 = Element_Index(pFluxRecord->ELEM1);
            idx2 = Element_Index(pFluxRecord->ELEM2);
            if(idx1>=mPTSAT.nElements-3)
            {
            }
            else if(abs(idx2-idx1)<=1||idx2>=mPTSAT.nElements-3) 
            {
              mPTSAT.X_VEL_AQ[idxT][idx1] = -pFluxRecord->FLO_AQ;
              mPTSAT.X_VEL_OIL[idxT][idx1] = -pFluxRecord->FLO_OIL;
            }
            else
            {
              mPTSAT.Y_VEL_AQ[idxT][idx1] = -pFluxRecord->FLO_AQ;
              mPTSAT.Y_VEL_OIL[idxT][idx1] = -pFluxRecord->FLO_OIL;
            }
          }
          idxT++;
#ifdef M64
          recLen = 0x20;
#else
          recLen = 0x10;
#endif
        }
        _close( fh1 );
      }
    }
    m_view.Invalidate();
  }
}

LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  int iTmp;
  CString tCstr;
	// TODO: add code to initialize document
  CFileDialog fileDlg ( true, _T("tpl"), NULL,
                      OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
                      _T("TOUGH2 Plots\0*.tp2;*.tpl\0All Files\0*.*\0") );
  if(!mPTSAT.sSelectedFile.IsEmpty())
  {
    iTmp = mPTSAT.sSelectedFile.ReverseFind(_T('\\'));
    if(iTmp>0)
    {
      tCstr = mPTSAT.sSelectedFile.Left(iTmp);
      fileDlg.m_ofn.lpstrInitialDir = tCstr;
    }
  }
  if ( IDOK == fileDlg.DoModal() )
    mPTSAT.sSelectedFile = fileDlg.m_szFileName;
  OpenPlot(mPTSAT.sSelectedFile);
	return 0;
}

LRESULT CMainFrame::OnFileReOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  if(!mPTSAT.sSelectedFile.IsEmpty())
    OpenPlot(mPTSAT.sSelectedFile);
  return 0;
}

LRESULT CMainFrame::OnFileSaveMovie(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // TODO: add code to initialize document

  CPaintDC dc(m_hWnd);
	CRect rc;
  GetClientRect(&rc);
  int size = min(rc.right*3/4,rc.bottom);
  int scaleFactor = PAINT_DIM/1.03/size;
  CPoint ptOrigin;
  ptOrigin.x = rc.right*1.5/10 ;
  ptOrigin.y = rc.bottom*9/10 ;
	CMemoryDC mDC(dc,rc);
	CBrush hBkGnd, hBr3;
	hBkGnd.CreateSysColorBrush(COLOR_WINDOW);
	hBr3.CreateSolidBrush(RGB(255,0,0));

  EncoderParameters *encoderParameters;
  ULONG             parameterValue;
  ULONG nCompression = EncoderValue::EncoderValueCompressionLZW;
  Status            stat;
  encoderParameters = (EncoderParameters*) malloc(sizeof(EncoderParameters)+2*sizeof(EncoderParameter));
  encoderParameters->Count = 2;
  // Initialize the one EncoderParameter object.
  encoderParameters->Parameter[0].Guid = EncoderSaveFlag;
  encoderParameters->Parameter[0].Type = EncoderParameterValueTypeLong;
  encoderParameters->Parameter[0].NumberOfValues = 1;
  encoderParameters->Parameter[0].Value = &parameterValue;
  encoderParameters->Parameter[1].Guid = EncoderCompression;
  encoderParameters->Parameter[1].Type = EncoderParameterValueTypeLong;
  encoderParameters->Parameter[1].NumberOfValues = 1;
  encoderParameters->Parameter[1].Value = &nCompression;

  CLSID encoderClsid;
  // Get image/tiff encoder clsid
  GetEncoderClsid(L"image/tiff", &encoderClsid);

  parameterValue = EncoderValueMultiFrame;
  int indTime = mPTSAT.indexTime;
  for(int i=0;i<mPTSAT.nTimes;i++)
  {
  	mDC.FillRect(&rc,hBkGnd);
    mPTSAT.indexTime = i;
    Paint((CDCHandle)mDC,NULL,scaleFactor,ptOrigin);
    Bitmap *bp = new Bitmap(mDC.GetCurrentBitmap().m_hBitmap,NULL);
    CString fname;
    fname.Format(_T("Output%03d.tiff"),i);
    stat = bp->Save(fname, &encoderClsid, NULL);
  }
  mPTSAT.indexTime = indTime;
  //parameterValue = EncoderValueFrameDimensionPage;
  //stat = bp->SaveAdd(bp, encoderParameters);
  //parameterValue = EncoderValueFlush;
  //stat = bp->SaveAdd(encoderParameters);

/*


  int scaleFactor, width, height;
  CRect rc;
  m_view.GetClientRect(&rc);
  CDC dc(m_view.GetDC());
  int nRes = dc.GetDeviceCaps(HORZRES);
  int nSize = dc.GetDeviceCaps(HORZSIZE);
  double dDPI = nRes*25.4/nSize;
  rc.left = 0; rc.right = 720; rc.top = 0; rc.bottom = 480;

  //CMemoryDC aDCmeta(m_view.GetDC(),rc);
  width = rc.right/25.4;
  height = rc.bottom/25.4;
	//dataBuf[0]=0;
	scaleFactor = PAINT_DIM/width*1.34*99.12195/dDPI; // a smaller number increases the size
	CPoint ptOrigin(width*0.15,height*0.9);

  Bitmap* bm = new Bitmap(rc.right,rc.bottom,PixelFormat24bppRGB);
  Graphics g(bm);
  HDC hdc = g.GetHDC();
  Paint((CDCHandle)hdc,NULL, scaleFactor, ptOrigin);
  g.ReleaseHDC(hdc);
  parameterValue = EncoderValueMultiFrame;
  stat = bm->Save(L"Output.tiff", &encoderClsid, &encoderParameters);
  parameterValue = EncoderValueFrameDimensionPage;
  stat = bm->SaveAdd(bm, &encoderParameters);
  parameterValue = EncoderValueFlush;
  stat = bm->SaveAdd(&encoderParameters);
  //Bitmap bm2 = new Bitmap(aDCmeta.m_bmp);
  //bm.GetBitmap
  //CEnhMetaFile mf(aDCmeta.Close());*/
  return 0;
}

LRESULT CMainFrame::OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // TODO: add code to initialize document
  int nLen, scaleFactor;
  CString tCstr;
  // Open the clipboard, and empty it. 
  if(!OpenClipboard()) return 1;
  ::EmptyClipboard();
  CEnhMetaFileDC aDCmeta;
  CRect rc;
  m_view.GetClientRect(&rc);
  CDC dc(m_view.GetDC());
  int nRes = dc.GetDeviceCaps(HORZRES);
  int nSize = dc.GetDeviceCaps(HORZSIZE);
  double dDPI = nRes*25.4/nSize;

  rc.left = 0; rc.right = 18200; rc.top = 0; rc.bottom = 13650;
  int tInt,width,height;
  aDCmeta.Create(m_view.GetDC(), NULL, &rc,NULL);
  width = rc.right/25.4;
  height = rc.bottom/25.4;
	tCstr.Empty();

  // Set the scale factor for proper Enhanced metafile rendering for MS Office
  // If the scale factor is not right, lines could be too thin or too thick
	scaleFactor = PAINT_DIM/width*1.34*99.12195/dDPI; // a smaller number increases the size
  // Set the origin of the plot
	CPoint ptOrigin(width*0.15,height*0.9);
  // Paint the saturation of oil or water
  Paint((CDCHandle)aDCmeta,&tCstr, scaleFactor, ptOrigin);
  // Set the painted output (vector graphics) to Clipboard
  ::SetClipboardData(CF_ENHMETAFILE,aDCmeta.Close());


  HGLOBAL hglbCopy; 
  wchar_t *lptstrCopy; 
  nLen = tCstr.GetLength();//nLen = wcslen(dataBuf);
  hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (nLen+1) * sizeof(wchar_t)); 
  if (hglbCopy == NULL) 
  { ::CloseClipboard();  return FALSE; } 
  // Lock the handle and copy the text to the buffer. 
  lptstrCopy = (wchar_t*)GlobalLock(hglbCopy);
  wcscpy(lptstrCopy, tCstr); 
  //lptstrCopy[nLen] = 0;    // null character 
  GlobalUnlock(hglbCopy); 
  // Place the handle on the clipboard. 
  SetClipboardData(CF_UNICODETEXT, hglbCopy); 
  ::CloseClipboard(); 
	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

void CMainFrame::OnToolBarCombo(HWND hWndCombo, UINT nID, int nSel, LPCTSTR lpszText, DWORD dwItemData)
{
	if (nID == ID_COMBO_PLACEHOLDER)
	{
    mPTSAT.indexTime = nSel;
    m_view.Invalidate();
	}
	else if (nID == ID_COMBO_PLACEHOLDER2)
	{
    mPTSAT.indexPlot = nSel;
    m_view.Invalidate();
	}
	else if (nID == ID_COMBO_PLACEHOLDER3)
	{
    mPTSAT.indexPlotType = nSel;
    if(nSel<2) m_wndComboTime.EnableWindow(1);
    else m_wndComboTime.EnableWindow(0);
    m_view.Invalidate();
	}
//	AtlMessageBox(*this, lpszText, IDR_MAINFRAME);
}

LRESULT CMainFrame::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
  if(wParam == 37) // Left Arrow
  {
    // go back 
    if(mPTSAT.indexTime>0)
    {
      mPTSAT.indexTime--;
      m_wndComboTime.SetCurSel(mPTSAT.indexTime);
      m_wndComboTime.Invalidate();
      m_view.Invalidate();
    }
    //bHandled = true;
  }
  else if(wParam == 39) // Right Arrow
  {
    // go forward
    if(mPTSAT.indexTime<mPTSAT.nTimes-1)
    {
      mPTSAT.indexTime++;
      m_wndComboTime.SetCurSel(mPTSAT.indexTime);
      m_wndComboTime.Invalidate();
      m_view.Invalidate();
    }
    //bHandled = true;
  }
  return 0;
}

LRESULT CMainFrame::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
  int xPos = GET_X_LPARAM(lParam); 
  int yPos = GET_Y_LPARAM(lParam);
  CString tCstr;
  int idx = mPTSAT.Find(xPos,yPos);
  int idT = mPTSAT.indexTime;
  if(idx>=0)
  {
    if(mPTSAT.indexPlotType ==0)
    {
      tCstr.Format(_T("%hs(%d)  %lf"),mPTSAT.ELEM[idx],mPTSAT.INDEX[idx],mPTSAT.GetSetPlotData(idx,0,0.0,false));
      m_wndStatusBar.SetPaneText(ID_PANE_VALUE,tCstr);
    }
    else if(mPTSAT.indexPlotType ==1)
    {
      tCstr.Format(_T("%hs(%d), xa %.3le, ya %.3le, xo %.3le, yo %.3le"),mPTSAT.ELEM[idx],mPTSAT.INDEX[idx],
        mPTSAT.X_VEL_AQ[idT][idx],mPTSAT.Y_VEL_AQ[idT][idx],mPTSAT.X_VEL_OIL[idT][idx],
        mPTSAT.Y_VEL_OIL[idT][idx]);
      m_wndStatusBar.SetPaneText(ID_PANE_VALUE,tCstr);
    }
  }
  else
  {
    m_wndStatusBar.SetPaneText(ID_PANE_VALUE,_T(""));
  }
	return 0;
}

LRESULT CMainFrame::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
  mPTSAT.bClientUpdated = false;
  bHandled = false;
	return 0;
}

int getCodeValue(char ch)
{
  if(ch==' ') return 0;
  else if(ch>='A') return ch-'A'+10;
  else return ch-'0';
}

int Element_Index(char buf[])//, int j, int i, int nxelem)
{
   int iTmp = getCodeValue(buf[0]);
   iTmp = iTmp*36+getCodeValue(buf[1]);
   iTmp = iTmp*36+getCodeValue(buf[2]);
   iTmp = iTmp*10+getCodeValue(buf[3]);
   iTmp = iTmp*10+getCodeValue(buf[4]);
   return iTmp;
}

// Get image encoder clsid
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
  UINT  num = 0;          // number of image encoders
  UINT  size = 0;         // size of the image encoder array in bytes

  ImageCodecInfo* pImageCodecInfo = NULL;
  // Obtain the total number and size of available image encoders
  GetImageEncodersSize(&num, &size);
  if (size == 0)
    return -1;  // Failure

  pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
  if (pImageCodecInfo == NULL)
    return -1;  // Failure
  //
  GetImageEncoders(num, size, pImageCodecInfo);

  for (UINT j = 0; j < num; ++j)
  {
    if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
    {
      *pClsid = pImageCodecInfo[j].Clsid;
      free(pImageCodecInfo);
      return j;  // Success
    }
  }

  free(pImageCodecInfo);
  return -1;  // Failure
}

