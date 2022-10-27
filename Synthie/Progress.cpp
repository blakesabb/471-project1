// Progress.cpp: implementation of the CProgress class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Synthie.h"
#include "Progress.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProgress::CProgress()
{

}

CProgress::~CProgress()
{

}

void CProgress::ProgressBegin(CWnd *pView)
{
   pView->EnableWindow(FALSE);   

   pView->BeginWaitCursor();
   m_progressdlg.Create(IDD_PROGRESS_DLG);
   m_progressdlg.ShowWindow(SW_SHOW);
   m_progressdlg.AbortClear();
}


void CProgress::ProgressEnd(CWnd *pView)
{
   m_progressdlg.DestroyWindow();
   pView->EndWaitCursor();
   pView->EnableWindow(TRUE);

}
