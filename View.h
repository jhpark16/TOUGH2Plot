// View.h : interface of the CView class
//
// Author: Jungho Park
// Date: July 2018
// Class definition for CView from Document/View architecture
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CView : public CWindowImpl<CView>, public CDynamicChain
{
public:
  DECLARE_WND_CLASS(NULL)

  // A function function pretranslate Windows messages for this view class
  // The MainFrm initially processes the message and then asks the view to translate the message
  BOOL PreTranslateMessage(MSG* pMsg)
  {
    pMsg;
    return FALSE;
  }
  // CDynamicChain allows CallChain(1521, xx)
  // This view class receives all the Windows message targeted to the client area.
  // If CView cannot handle the message, use CallChain to send the message to CMainFrm.
  // Since CView cannot update the Toolbar or Statusbar, any operations updating these 
  // should be sent to CMainFrm.
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

// Definitions for maximum allowed sizes for the plot data
#define MAX_RECORD 2000
// Maximum number of time steps allowed
#define MAX_TIME 330
// Maximum number of elements from the model
#define MAX_ELEM 16500
// Target virtual dimension of the plot
#define PAINT_DIM 100000

#pragma pack(push,1) 

// When you compile the data structure for 64 bit integers, use M64 defintion
// The current TOUGH2 plot example files are designed for 32 bit integers.
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
#else // Structure for 32 bit integers
// Structure for the header of TOUGH2 Pressure, temperature, saturation plot data. 
// The current TOUGH2 plot example files (FLUX.TPL and PTSAT.TPL) are designed 
// for 32 bit integers.
// The TOUGH2 plot data (PTSAT.TPL) begins with this header structure.
struct T2PTSATHead
{
  // Number of total elements(cells)
  int NEL;
  // The final time step
  float SUMTIM;
  // Number of cyles
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
  // 
  float DELTEX;
};
// A record for one cell
struct T2PTSATRecord
{
  // The name (ID) of element(cell)
  char ELEM[5];
  // The numerical index of the element starting from 0
  int INDEX;
  // The pressure of the cell in Pascal
  float PRESSURE;
  // The temperature in Celcius
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
  // The name (ID) of element #1(cell)
  char ELEM1[5];
  // The name (ID) of element #2(cell)
  char ELEM2[5];
  // The numerical index of this record starting from 0
  int INDEX;
  // Flow of heat
  float FLOH;
  // Flow of heat and flux
  float FLOH_FLOF;
  // Flux of total flow
  float FLOF;
  // Flow of gas in kg/s
  float FLO_GAS;
  // Flow of aqueous fraction in kg/s
  float FLO_AQ; // kg/s
  // Velocity of gas fraction in m/s
  float VEL_GAS;
  // Velocity of aqueous fraction in m/s
  float VEL_AQ;
  // flow of oil fraction in m/s
  float FLO_OIL; // kg/s
};
#endif
#pragma pack(pop) 

// Rectangle geometry class in double precision
class CRectD
{
public:
  double left;
  double right;
  double top;
  double bottom;
};

// Main data structure
class PTSATRecord
{
public:
  // The file got from the data structure
  CString sSelectedFile;
  // The index of the current time step
  int indexTime;
  // The current plot
  int indexPlot;
  // The current plot type
  int indexPlotType;
  // Number of time steps
  int nTimes;
  // Number of elements
  int nElements;
  // Number of connections between elements
  int nConnection;
  // Number of rows of the model
  int nRows;
  // Number of columns of the model
  int nColumns;
  // 
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
  // Velocity of aqueous fraction along X direction of each element
  // at each time step
  double X_VEL_AQ[MAX_TIME][MAX_ELEM];
  // Velocity of aqueous fraction along Y direction of each element
  double Y_VEL_AQ[MAX_TIME][MAX_ELEM];
  // Velocity of oil fraction along X direction of each element
  double X_VEL_OIL[MAX_TIME][MAX_ELEM];
  // Velocity of oil fraction along Y direction of each element
  double Y_VEL_OIL[MAX_TIME][MAX_ELEM];

  // Find the element using the x, y mouse locations and returns
  // the index of the element
  int Find(int ix, int iy)
  {
    for (int i = 0; i < nElements; i++)
    {
      if (boxClient[i].PtInRect(CPoint(ix, iy)))
      {
        // Returns the index of the element (cell)
        return i;
      }
    }
    // If the mouse location is on colour map, returns -2
    if (boxCMapLegend.PtInRect(CPoint(ix, iy))) return -2;
    // Returns -1 when failed
    return -1;
  };

  // Get or Set the plot data for state variables (Temperature, Saturation of liquid, gas, and oil, etc)
  // idx
  // If bSet is true, the val is set to the variable
  double GetSetPlotData(int idx, int iData, double val, bool bSet)
  {
    switch (indexPlot) // The current plot type
    {
    case 1: // If the current plot type is the saturation of liquid  (water)
      switch (iData)
      {
      case 1: case 3: { if (bSet) SL_RANGE[iData - 1] = val; return SL_RANGE[iData - 1]; }
      case 2: case 4: { if (bSet) SL_RANGE[iData - 1] = val; return SL_RANGE[iData - 1]; }
      case 5: return SL_Avg[idx];
      case 0:
      default: return SL[indexTime][idx];
      }
    case 2: // If the current plot type is the Saturation of gas
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
        // For any other cases, returns the current SOIL value of the cell (element) at idx
      case 0:
      default: return ln_K[idx];
      }
    case 0:
    default:
      switch (iData)
      {
      case 1: case 3: { if (bSet) SOIL_RANGE[iData - 1] = val; return SOIL_RANGE[iData - 1]; }
      case 2: case 4: { if (bSet) SOIL_RANGE[iData - 1] = val; return SOIL_RANGE[iData - 1]; }
              // If iData is 5, returns the average of saturaiton of oil
      case 5: return SOIL_Avg[idx];
        // For any other cases, returns the current SOIL value of the cell (element) at idx
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

