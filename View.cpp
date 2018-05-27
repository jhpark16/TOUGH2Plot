// View.cpp : implementation of the CView class
//
// Author: Jungho Park
// Date: July 2018
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "View.h"
#include "rangedlg.h"

extern PTSATRecord mPTSAT;

// Return the value of the colour map (jet scheme) for the level
COLORREF CCMap(double level)
{
  double lvl;
  int R,G,B;
  level = max(0.0, min(level, 1.0));
  // Divide the level into 8 intervals
  lvl = level*8.0;
  if(lvl<=1.0)
  {
    // Starting from B=127 and increase B with increasing level
    R=0; G=0;
    B = (int)(127.5*(lvl)+127.5);
  }
  else if(lvl<=3.0)
  {
    // From B=255, decrease B with increasing level
    R=0; B=255;
    G = (int)(127.5*(lvl-1.0));
  }
  else if(lvl<=5.0)
  {
    G=255;
    // From Cyan to Yello with increasing level
    B = (int)(127.5*(5.0-lvl));
    R = (int)(127.5*(lvl-3.0));
  }
  else if(lvl<=7.0)
  {
    R=255; B=0;
    // From Yello to Red with increasing level
    G = (int)(127.5*(7.0-lvl));
  }
  else
  {
    G=0; B=0;
    // From Red to R=127 with increasing level
    R = (int)(127.5*(8.0-lvl)+127.5);
  }
  // Limit R, G, B to unsigned character range
  R = max(0, min(R, 255));
  G = max(0, min(G, 255));
  B = max(0, min(B, 255));
  return RGB(R,G,B);
}

// Scale a rentangle by the scaleFactor and add ptOrigin value
void ScaleRect(CRect *rect,int scaleFactor, CPoint ptOrigin)
{
  rect->left = rect->left/scaleFactor+ptOrigin.x;
  rect->right = rect->right/scaleFactor+ptOrigin.x;
  rect->top = -rect->top/scaleFactor+ptOrigin.y;
  rect->bottom = -rect->bottom/scaleFactor+ptOrigin.y;
  rect->NormalizeRect();
}

// Scale a point by the scaleFactor and add ptOrigin value
void ScalePoint(CPoint *point,int scaleFactor, CPoint ptOrigin)
{
  point->x = point->x/scaleFactor+ptOrigin.x;
  point->y = -point->y/scaleFactor+ptOrigin.y;
}

// Main paint routine to draw the plot using the device context (dc)
// and scaleFactor and ptOrign (the amount of 2D shift)
// wbuf returns the plot data as text separated with tabs for Clipboard
void Paint(CDCHandle dc, CString *wbuf, int scaleFactor, CPoint ptOrigin)
{
	//TODO: Add your drawing code here
  CString tCstr;
  double avg_val=0.0,min_val,max_val;
  int idxT = mPTSAT.indexTime, tInt, fontPoint=PAINT_DIM/33;
  CPen cpR,cpG,cpB,cpBlkThick,cpBlk;
  cpR.CreatePen(PS_SOLID,400/scaleFactor,RGB(255,96,96));
  cpG.CreatePen(PS_SOLID,400/scaleFactor,RGB(0,255,0));
  cpB.CreatePen(PS_SOLID,400/scaleFactor,RGB(96,96,255));
  cpBlkThick.CreatePen(PS_SOLID,400/scaleFactor,RGB(0,0,0));
  cpBlk.CreatePen(PS_SOLID,200/scaleFactor,RGB(0,0,0));
  dc.SelectPen(cpBlk);
  CRect rc, tRect;
  CPoint tPoint1, tPoint2;
  CBrush cb;
  CFont fnt;
  CSize sz;
  fnt.CreateFontW(-fontPoint/scaleFactor,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,_T("Arial"));
  dc.SelectFont(fnt);
  mPTSAT.nArrowInterval = 4;
  if(mPTSAT.indexPlotType<2)
  {
    if(mPTSAT.nTimes>0 || mPTSAT.nElements>0 && mPTSAT.indexPlot==5)
    {
      //mPTSAT.indexPlot = 3;
      if(mPTSAT.indexPlot == 10)// 7 for log K
      {
        min_val = -3;
        max_val = 3;
      }
      else
      {
        min_val = mPTSAT.GetSetPlotData(0,1,0.0,false);
        max_val = mPTSAT.GetSetPlotData(0,2,0.0,false);
      }
      double tval, xLocOld;
      int itmp;
      xLocOld = -1e10;
      if(mPTSAT.indexPlotType==1)
      {
        tRect = CRect((mPTSAT.BoundBox.left+mPTSAT.DX)*mPTSAT.xscale,mPTSAT.BoundBox.top*mPTSAT.yscale,
          (mPTSAT.BoundBox.right-mPTSAT.DX*2)*mPTSAT.xscale,mPTSAT.BoundBox.bottom*mPTSAT.yscale);
        ScaleRect(&tRect,scaleFactor,ptOrigin);
        dc.MoveTo(tRect.left,tRect.bottom);
        dc.LineTo(tRect.right,tRect.bottom);
        dc.LineTo(tRect.right,tRect.top);
        dc.LineTo(tRect.left,tRect.top);
        dc.LineTo(tRect.left,tRect.bottom);
      }
      for(int i=0;i<mPTSAT.nElements;i++)
      {
        tRect = mPTSAT.box[i];
        ScaleRect(&tRect,scaleFactor,ptOrigin);
        if(!mPTSAT.bClientUpdated)
        {
          mPTSAT.boxClient[i] = tRect;
        }
        if(mPTSAT.indexPlotType==0)
        {
          tval = mPTSAT.GetSetPlotData(i,0,0.0,false);
          if(wbuf!=NULL)
          {
            if(mPTSAT.X[i]<xLocOld)
            {
              wbuf->AppendChar(_T('\n'));
              //int len = wbuf->GetLength();
              //wbuf[len-1]=_T('\n');
              //wbuf[len]=0;
            }
            xLocOld=mPTSAT.X[i];
            tCstr.Format(_T("%lf\t"),tval);
            //swprintf(tstr,_T("%lf\t"),tval);
            *wbuf += tCstr;
            //wcscat(wbuf,tCstr);//tstr);
          }
          cb.CreateSolidBrush(CCMap((tval-min_val)/(max_val-min_val)));
          tInt=dc.FillRect(&tRect,cb);
          cb.DeleteObject();
        }
        else if(i%mPTSAT.nArrowInterval==0 && 
          (i/mPTSAT.nColumns)%mPTSAT.nArrowInterval==0&& i<mPTSAT.nElements-1)
        {
          // vector velocity plot
          double sca_factor = 2.5e12;
          if((i/mPTSAT.nArrowInterval)%2 ==0)
            dc.SelectPen(cpR); 
          else if ((i/mPTSAT.nArrowInterval)%2 ==1)
            dc.SelectPen(cpBlkThick); 
          //
          int x = (tRect.left+tRect.right)/2;
          int y = (tRect.top+tRect.bottom)/2;
          dc.MoveTo(x,y);
          int x1, y1;
          double angle, magnitude;
          if(mPTSAT.indexPlot==0)
          {
            x1 = x + mPTSAT.X_VEL_OIL[idxT][i]/scaleFactor*sca_factor;
            y1 = y - mPTSAT.Y_VEL_OIL[idxT][i]/scaleFactor*sca_factor;
            angle = 90.0-atan2(mPTSAT.Y_VEL_OIL[idxT][i],mPTSAT.X_VEL_OIL[idxT][i])/M_PI*180.0;
            magnitude = sqrt(pow(mPTSAT.X_VEL_OIL[idxT][i],2)+pow(mPTSAT.Y_VEL_OIL[idxT][i],2))*sca_factor;
          }
          else
          {
            x1 = x + mPTSAT.X_VEL_AQ[idxT][i]/scaleFactor*sca_factor;
            y1 = y - mPTSAT.Y_VEL_AQ[idxT][i]/scaleFactor*sca_factor;
            angle = 90.0-atan2(mPTSAT.Y_VEL_AQ[idxT][i],mPTSAT.X_VEL_AQ[idxT][i])/M_PI*180.0;
            magnitude = sqrt(pow(mPTSAT.X_VEL_AQ[idxT][i],2)+pow(mPTSAT.Y_VEL_AQ[idxT][i],2))*sca_factor;
          }
          if(wbuf!=NULL)
          {
            tCstr.Format(_T("%le\t%le\t%le\t%le\n"),mPTSAT.X[i],mPTSAT.Y[i],angle, sqrt(magnitude));
            *wbuf += tCstr;
          }
          dc.LineTo(x1,y1);
          //dc.DrawEdge(&tRect,,BF_FLAT); 
        }
      }
      //avg_val /= (mPTSAT.nElements-3);
      avg_val = mPTSAT.GetSetPlotData(idxT,5,0.0,false);
      rc.left = mPTSAT.BoundBox.right*mPTSAT.xscale+PAINT_DIM*0.06;
      rc.right = rc.left+PAINT_DIM*0.05;
      int nMap = 200;
      double bandHeight = (mPTSAT.BoundBox.bottom-mPTSAT.BoundBox.top)*mPTSAT.yscale/(double) nMap;
      double legendTop = mPTSAT.BoundBox.top*mPTSAT.yscale;
      if(mPTSAT.indexPlotType==0)
      {
        for(int i=0;i<nMap;i++)
        {
          rc.top = legendTop + i*bandHeight;
          rc.bottom = rc.top + bandHeight;
          cb.CreateSolidBrush(CCMap(i/(double)nMap));
          tRect = rc;
          ScaleRect(&tRect,scaleFactor,ptOrigin);
          tInt=dc.FillRect(&tRect,cb);
          cb.DeleteObject();
        }
      }
      if(!mPTSAT.bClientUpdated)
      {
        rc.top = legendTop;
        rc.bottom = rc.top + bandHeight*100;
        tRect = rc;
        ScaleRect(&tRect,scaleFactor,ptOrigin);
        mPTSAT.boxCMapLegend = tRect;
      }
      mPTSAT.bClientUpdated = true;
      dc.SelectPen(cpBlk);
      if(mPTSAT.indexPlotType==0)
      {
        tCstr.Format(_T("%.2lf"),min_val);
        dc.GetTextExtent(tCstr,tCstr.GetLength(),&sz);
        tRect = CRect(rc.right+PAINT_DIM*0.007,mPTSAT.BoundBox.top*mPTSAT.yscale,
          rc.right+PAINT_DIM*0.007,mPTSAT.BoundBox.top*mPTSAT.yscale);
        ScaleRect(&tRect,scaleFactor,ptOrigin);
        tRect.right = tRect.left+sz.cx;
        tRect.top -= sz.cy/2.0;
        tRect.bottom = tRect.top+sz.cy;
        tInt=dc.DrawTextW(tCstr,-1,&tRect,0);

        tCstr.Format(_T("%.2lf"),(min_val+max_val)/2.0);
        dc.GetTextExtent(tCstr,tCstr.GetLength(),&sz);
        tRect = CRect(rc.right+PAINT_DIM*0.007,(mPTSAT.BoundBox.top+mPTSAT.BoundBox.bottom)/2.0*mPTSAT.yscale,
          rc.right+PAINT_DIM*0.007,(mPTSAT.BoundBox.top+mPTSAT.BoundBox.bottom)/2.0*mPTSAT.yscale);
        ScaleRect(&tRect,scaleFactor,ptOrigin);
        tRect.right = tRect.left+sz.cx;
        tRect.top -= sz.cy/2.0;
        tRect.bottom = tRect.top+sz.cy;
        tInt=dc.DrawTextW(tCstr,-1,&tRect,0);
        
        tCstr.Format(_T("%.2lf"),max_val);
        dc.GetTextExtent(tCstr,tCstr.GetLength(),&sz);
        tRect = CRect(rc.right+PAINT_DIM*0.007,mPTSAT.BoundBox.bottom*mPTSAT.yscale,
          rc.right+PAINT_DIM*0.007,mPTSAT.BoundBox.bottom*mPTSAT.yscale);
        ScaleRect(&tRect,scaleFactor,ptOrigin);
        tRect.right = tRect.left+sz.cx;
        tRect.top -= sz.cy/2.0;
        tRect.bottom = tRect.top+sz.cy;
        tInt=dc.DrawTextW(tCstr,-1,&tRect,0);
      }
//      if(mPTSAT.indexPlotType==1)
//        swprintf(tstr,_T("Time (%d):%.0lf(sec)"),idxT,mPTSAT.Time[idxT]);
//      else 
      if(fabs(avg_val)>0.001 && fabs(avg_val)<1000)
        tCstr.Format(_T("Time (%d):%le(sec),  Avg value(%d):%lf"),idxT,mPTSAT.Time[idxT],mPTSAT.nElements-3,avg_val);
      else
        tCstr.Format(_T("Time (%d):%le(sec),  Avg value(%d):%le"),idxT,mPTSAT.Time[idxT],mPTSAT.nElements-3,avg_val);
      dc.GetTextExtent(tCstr,tCstr.GetLength(),&sz);
      tRect = CRect(PAINT_DIM*-0.05,PAINT_DIM/4*3*1.05,
        PAINT_DIM*-0.05,PAINT_DIM/4*3*1.05);
      ScaleRect(&tRect,scaleFactor,ptOrigin);
      tRect.right = tRect.left+sz.cx;
      tRect.top = tRect.bottom - sz.cy;
      tInt=dc.DrawTextW(tCstr,-1,&tRect,0);
    }
  }
  else
  {
    tPoint1 = CPoint(0,0); ScalePoint(&tPoint1, scaleFactor, ptOrigin);
    dc.MoveTo(tPoint1);
    tPoint1 = CPoint(PAINT_DIM,0); ScalePoint(&tPoint1, scaleFactor, ptOrigin);
    dc.LineTo(tPoint1);
    tPoint1 = CPoint(PAINT_DIM,PAINT_DIM/4*3); ScalePoint(&tPoint1, scaleFactor, ptOrigin);
    dc.LineTo(tPoint1);
    tPoint1 = CPoint(0,PAINT_DIM/4*3); ScalePoint(&tPoint1, scaleFactor, ptOrigin);
    dc.LineTo(tPoint1);
    tPoint1 = CPoint(0,0); ScalePoint(&tPoint1, scaleFactor, ptOrigin);
    dc.LineTo(tPoint1);
    double tRange = mPTSAT.Time[mPTSAT.nTimes-1];
    min_val = mPTSAT.GetSetPlotData(0,1,0.0,false);
    max_val = mPTSAT.GetSetPlotData(0,2,0.0,false);
    double val_range = max_val-min_val;
    for(int i=0;i<mPTSAT.nTimes;i++)
    {
      double x = mPTSAT.Time[i]/tRange*PAINT_DIM;
      double tval = mPTSAT.GetSetPlotData(i,5,0.0,false);
      double y = (tval-min_val)/val_range*PAINT_DIM/4*3;
      if(i==0)
      {
        tPoint1 = CPoint(x,y); ScalePoint(&tPoint1, scaleFactor, ptOrigin);
        dc.MoveTo(tPoint1);
      }
      else
      {
        tPoint1 = CPoint(x,y); ScalePoint(&tPoint1, scaleFactor, ptOrigin);
        dc.LineTo(tPoint1);
      }
      if(wbuf!=NULL)
      {
        tCstr.Format(_T("%lf\t%lf\n"),mPTSAT.Time[i],tval);
        *wbuf += tCstr;
      }
    }
    tCstr.Format(_T("%.3lf"),min_val);
    dc.GetTextExtent(tCstr,tCstr.GetLength(),&sz);
    tRect = CRect(-PAINT_DIM*0.01,0,-PAINT_DIM*0.01,0);
    ScaleRect(&tRect,scaleFactor,ptOrigin);
    tRect.left = tRect.right-sz.cx;
    tRect.top -= sz.cy/2.0;
    tRect.bottom = tRect.top+sz.cy;
    tInt=dc.DrawTextW(tCstr,-1,&tRect,0);

    tCstr.Format(_T("%.3lf"),(min_val+max_val)/2);
    dc.GetTextExtent(tCstr,tCstr.GetLength(),&sz);
    tRect = CRect(-PAINT_DIM*0.01,PAINT_DIM/4*3/2,-PAINT_DIM*0.01,PAINT_DIM/4*3/2);
    ScaleRect(&tRect,scaleFactor,ptOrigin);
    tRect.left = tRect.right-sz.cx;
    tRect.top -= sz.cy/2;
    tRect.bottom = tRect.top+sz.cy;
    tInt=dc.DrawTextW(tCstr,-1,&tRect,0);

    tCstr.Format(_T("%.3lf"),max_val);
    dc.GetTextExtent(tCstr,tCstr.GetLength(),&sz);
    tRect = CRect(-PAINT_DIM*0.01,PAINT_DIM/4*3,-PAINT_DIM*0.01,PAINT_DIM/4*3);
    ScaleRect(&tRect,scaleFactor,ptOrigin);
    tRect.left = tRect.right-sz.cx;
    tRect.top -= sz.cy/2;
    tRect.bottom = tRect.top+sz.cy;
    tInt=dc.DrawTextW(tCstr,-1,&tRect,0);
    
    tCstr.Format(_T("%.0lf"),0.0);
    dc.GetTextExtent(tCstr,tCstr.GetLength(),&sz);
    tRect = CRect(0-sz.cx/2,-100,0+sz.cx/2,-100-sz.cy);
    tRect = CRect(0,-PAINT_DIM*3/4*0.01,0,-PAINT_DIM*3/4*0.01);
    ScaleRect(&tRect,scaleFactor,ptOrigin);
    tRect.left += -sz.cx/2;
    tRect.right = tRect.left+sz.cx;
    tRect.bottom = tRect.top+sz.cy;
    tInt=dc.DrawTextW(tCstr,-1,&tRect,0);

    tCstr.Format(_T("%.0lf"),tRange/2.0);
    dc.GetTextExtent(tCstr,tCstr.GetLength(),&sz);
    tRect = CRect(PAINT_DIM/2.0,-PAINT_DIM*3/4*0.01,PAINT_DIM/2.0,-PAINT_DIM*3/4*0.01);
    ScaleRect(&tRect,scaleFactor,ptOrigin);
    tRect += CRect(-sz.cx / 2, 0, sz.cx / 2, sz.cy);
    //tRect.left += -sz.cx/2;
    //tRect.right = tRect.left+sz.cx;
    //tRect.bottom = tRect.top+sz.cy;
    tInt=dc.DrawTextW(tCstr,-1,&tRect,0);

    tCstr.Format(_T("%.0lf"),tRange);
    dc.GetTextExtent(tCstr,tCstr.GetLength(),&sz);
    tRect = CRect(PAINT_DIM,-PAINT_DIM*3/4*0.01,PAINT_DIM,-PAINT_DIM*3/4*0.01);
    ScaleRect(&tRect,scaleFactor,ptOrigin);
    tRect.left += -sz.cx/2;
    tRect.right = tRect.left+sz.cx;
    tRect.bottom = tRect.top+sz.cy;
    tInt=dc.DrawTextW(tCstr,-1,&tRect,0);
  }
}

LRESULT CView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
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
	mDC.FillRect(&rc,hBkGnd);
  Paint((CDCHandle)mDC,NULL,scaleFactor,ptOrigin);
  dc.BitBlt(rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,mDC,0,0,SRCCOPY);

	return 0;
}

LRESULT CView::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  // prevent erase background (inform that it is handled here)
	return 1;
}


LRESULT CView::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
  // If Left arrow (37) or Right arrow (39) is pressed
  if(wParam == 37 || wParam == 39)
  {
    LRESULT lResult;
    // Send the message to the MainFrm message handler
    CallChain(1521, m_hWnd, uMsg, wParam, lParam, lResult);
    // When message is processed by handler, it is marked as handled.
    // So, this is not necessary
    //SetMsgHandled(true);
  }
	return 0;
}

LRESULT CView::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	TRACKMOUSEEVENT mevt;
	mevt.cbSize = sizeof(TRACKMOUSEEVENT);
	mevt.dwFlags = TME_HOVER;
	mevt.dwHoverTime = HOVER_DEFAULT;
	mevt.hwndTrack = m_hWnd;
	int nn=::TrackMouseEvent(&mevt);
	return 0;
}

// MouseOver message is processed by the CMainFrm
LRESULT CView::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
  LRESULT lResult;
  if(mPTSAT.bClientUpdated)
  { CallChain(1521, m_hWnd, uMsg, wParam, lParam, lResult); }
  else
    SetMsgHandled(false); // In this case, the message should be handled by the other message handlers
  return 0;
}

// If a user double clicks the colour map with the left mouse button,
// a dialog to set the range of plot data will be shown. 
LRESULT CView::OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
  int xPos = GET_X_LPARAM(lParam); 
  int yPos = GET_Y_LPARAM(lParam);
  // Get the type of the object clicked using the mouse position.
  int idx = mPTSAT.Find(xPos,yPos);
  // If the clicked object is the colour map,
  if(idx==-2)
  {
    CRangeDlg dlg;
    // Shows the range set dialog
    dlg.DoModal();
    // Get minimum and maximum values from the dialog box
    mPTSAT.GetSetPlotData(0,1,dlg.m_minval,true);
    mPTSAT.GetSetPlotData(0,2,dlg.m_maxval,true);
    // Redraw the plot to reflect the new data range
    Invalidate();
  }
  return 0;
}
