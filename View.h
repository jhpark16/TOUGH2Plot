// View.h : interface of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CView : public CWindowImpl<CView>, public CDynamicChain
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
    MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
    MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
    MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
    MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover)
    MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
    //CHAIN_MSG_MAP_DYNAMIC(1521)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
  LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnMouseHover(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

};

#define MAX_RECORD 2000
#define MAX_TIME 330
#define MAX_ELEM 16500
#define PAINT_DIM 100000

#pragma pack(push,1) 

//#define M64
#ifdef M64
struct T2PTSATHead
{
  __int64 NEL;
  double SUMTIM;
  __int64 KCYC;
  __int64 ITER;
  __int64 ITERC;
  __int64 KON;
  double DX1M;
  double DX2M;
  double DX3M;
  double MAX_REDISUAL;
  __int64 NER;
  __int64 KER;
  double DELTEX;
};

struct T2PTSATRecord
{
  char ELEM[5];
  __int64 INDEX;
  double PRESSURE;
  double TEMPERATURE;
  double SG;
  double SL;
  double SOIL;
  double VISCOSITY;
  double PCAP;
};

struct T2FluxHead
{
  __int64 NCON;
  __int64 KCYC;
  __int64 ITER;
  double SUMTIM;
};
struct T2FluxRecord
{
  char ELEM1[5];
  char ELEM2[5];
  __int64 INDEX;
  double FLOH;
  double FLOH_FLOF;
  double FLOF;
  double FLO_GAS;
  double FLO_AQ; // kg/s
  double VEL_GAS; // m/s
  double VEL_AQ; // m/s
  double FLO_OIL; // kg/s
};
#else
struct T2PTSATHead
{
  int NEL;
  float SUMTIM;
  int KCYC;
  int ITER;
  int ITERC;
  int KON;
  float DX1M;
  float DX2M;
  float DX3M;
  float MAX_REDISUAL;
  int NER;
  int KER;
  float DELTEX;
};

struct T2PTSATRecord
{
  char ELEM[5];
  int INDEX;
  float PRESSURE;
  float TEMPERATURE;
  float SG;
  float SL;
  float SOIL;
  float VISCOSITY;
  float PCAP;
};

struct T2FluxHead
{
  int NCON;
  int KCYC;
  int ITER;
  float SUMTIM;
};
struct T2FluxRecord
{
  char ELEM1[5];
  char ELEM2[5];
  int INDEX;
  float FLOH;
  float FLOH_FLOF;
  float FLOF;
  float FLO_GAS;
  float FLO_AQ; // kg/s
  float VEL_GAS; // m/s
  float VEL_AQ; // m/s
  float FLO_OIL; // kg/s
};
#endif
#pragma pack(pop) 

class CRectD
{
public:
  double left;
  double right;
  double top;
  double bottom;
};

class PTSATRecord
{
public:
  CString sSelectedFile;
  int indexTime;
  int indexPlot;
  int indexPlotType;
  int nTimes;
  int nElements;
  int nConnection;
  int nRows;
  int nColumns;
  int nArrowInterval;
  bool bClientUpdated;
  double xscale, yscale;
  double DX, DY;
  double Time[MAX_TIME];
  int Cycle[MAX_TIME];
  char ELEM[MAX_ELEM][6];
  int INDEX[MAX_ELEM];
  CRectD BoundBox;
  CRectD boxD[MAX_ELEM];
  CRect boxClient[MAX_ELEM];
  CRect boxCMapLegend;
  CRect box[MAX_ELEM];
  double X[MAX_ELEM];
  double Y[MAX_ELEM];
  double Z[MAX_ELEM];
  double Z_top[MAX_ELEM];
  double Z_bottom[MAX_ELEM];
  double ln_K_RANGE[4];
  double ln_K[MAX_ELEM];
  double ln_K_Avg;
  double PRESSURE_RANGE[4];
  double PRESSURE_Avg[MAX_TIME];
  double PRESSURE[MAX_TIME][MAX_ELEM];
  double TEMPERATURE_RANGE[4];
  double TEMPERATURE_Avg[MAX_TIME];
  double TEMPERATURE[MAX_TIME][MAX_ELEM];
  double SG_RANGE[4];
  double SG_Avg[MAX_TIME];
  double SG[MAX_TIME][MAX_ELEM];
  double SL_RANGE[4];
  double SL_Avg[MAX_TIME];
  double SL[MAX_TIME][MAX_ELEM];
  double SOIL_RANGE[4];
  double SOIL_Avg[MAX_TIME];
  double SOIL[MAX_TIME][MAX_ELEM];
  double PCAP_RANGE[4];
  double PCAP_Avg[MAX_TIME];
  double PCAP[MAX_TIME][MAX_ELEM];
  double P_REAL_RANGE[4];
  double P_REAL_Avg[MAX_TIME];
  double X_VEL_AQ[MAX_TIME][MAX_ELEM];
  double Y_VEL_AQ[MAX_TIME][MAX_ELEM];
  double X_VEL_OIL[MAX_TIME][MAX_ELEM];
  double Y_VEL_OIL[MAX_TIME][MAX_ELEM];
  int Find(int ix, int iy)
  {
    for (int i = 0; i<nElements; i++)
    {
      if (boxClient[i].PtInRect(CPoint(ix, iy)))
      {
        return i;
      }
    }
    if (boxCMapLegend.PtInRect(CPoint(ix, iy))) return -2;
    return -1;
  };
  double GetSetPlotData(int idx, int iData, double val, bool bSet)
  {
    switch (indexPlot)
    {
    case 1:
      switch (iData)
      {
      case 1: case 3: { if (bSet) SL_RANGE[iData - 1] = val; return SL_RANGE[iData - 1]; }
      case 2: case 4: { if (bSet) SL_RANGE[iData - 1] = val; return SL_RANGE[iData - 1]; }
      case 5: return SL_Avg[idx];
      case 0:
      default: return SL[indexTime][idx];
      }
    case 2:
      switch (iData)
      {
      case 1: case 3: { if (bSet) SG_RANGE[iData - 1] = val; return SG_RANGE[iData - 1]; }
      case 2: case 4: { if (bSet) SG_RANGE[iData - 1] = val; return SG_RANGE[iData - 1]; }
      case 5: return SG_Avg[idx];
      case 0:
      default: return SG[indexTime][idx];
      }
    case 3:
      switch (iData)
      {
      case 1: case 3: { if (bSet) TEMPERATURE_RANGE[iData - 1] = val; return TEMPERATURE_RANGE[iData - 1]; }
      case 2: case 4: { if (bSet) TEMPERATURE_RANGE[iData - 1] = val; return TEMPERATURE_RANGE[iData - 1]; }
      case 5: return TEMPERATURE_Avg[idx];
      case 0:
      default: return TEMPERATURE[indexTime][idx];
      }
    case 4:
      switch (iData)
      {
      case 1: case 3: { if (bSet) PRESSURE_RANGE[iData - 1] = val; return PRESSURE_RANGE[iData - 1]; }
      case 2: case 4: { if (bSet) PRESSURE_RANGE[iData - 1] = val; return PRESSURE_RANGE[iData - 1]; }
      case 5: return PRESSURE_Avg[idx];
      case 0:
      default: return PRESSURE[indexTime][idx];
      }
    case 5:
      switch (iData)
      {
      case 1: case 3: { if (bSet) PCAP_RANGE[iData - 1] = val; return PCAP_RANGE[iData - 1]; }
      case 2: case 4: { if (bSet) PCAP_RANGE[iData - 1] = val; return PCAP_RANGE[iData - 1]; }
      case 5: return PCAP_Avg[idx];
      case 0:
      default: return PCAP[indexTime][idx];
      }
    case 6:
      switch (iData)
      {
      case 1: case 3: { if (bSet) P_REAL_RANGE[iData - 1] = val; return P_REAL_RANGE[iData - 1]; }
      case 2: case 4: { if (bSet) P_REAL_RANGE[iData - 1] = val; return P_REAL_RANGE[iData - 1]; }
      case 5: return P_REAL_Avg[idx];
      case 0:
      default: return PRESSURE[indexTime][idx] + PCAP[indexTime][idx];
      }
    case 7:
      switch (iData)
      {
      case 1: case 3: { if (bSet) ln_K_RANGE[iData - 1] = val; return ln_K_RANGE[iData - 1]; }
      case 2: case 4: { if (bSet) ln_K_RANGE[iData - 1] = val; return ln_K_RANGE[iData - 1]; }
      case 5: return ln_K_Avg;
      case 0:
      default: return ln_K[idx];
      }
    case 0:
    default:
      switch (iData)
      {
      case 1: case 3: { if (bSet) SOIL_RANGE[iData - 1] = val; return SOIL_RANGE[iData - 1]; }
      case 2: case 4: { if (bSet) SOIL_RANGE[iData - 1] = val; return SOIL_RANGE[iData - 1]; }
      case 5: return SOIL_Avg[idx];
      case 0:
      default: return SOIL[indexTime][idx];
      }
    }
  }
};

COLORREF CCMap(double level);
int FetchARecord(int fh1, char *buffer, unsigned int *recLen);
int ParseDelimiters(wchar_t* pStr, wchar_t* pDelimeters, wchar_t *pwch[], int maxN);
void ScaleRect(CRect *rect, int scaleFactor, CPoint ptOrigin);
void ScalePoint(CPoint *point, int scaleFactor, CPoint ptOrigin);
void Paint(CDCHandle dc, CString *wbuf, int scaleFactor, CPoint ptOrigin);
int getCodeValue(char ch);
int Element_Index(char *buf);
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

