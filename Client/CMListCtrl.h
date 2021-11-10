#pragma once
#include "afxcmn.h"
#include "MFCApplication5.h"
typedef struct
{
    COLORREF colText;
    COLORREF colTextBk;
}TEXT_BK;

class CMListCtrl :
    public CListCtrl
{
public:
    CMListCtrl();
    CEdit *m_edit;
    int m_index;
    int m_idSort, m_idCmp;
    CMap<DWORD, DWORD&, TEXT_BK, TEXT_BK&> MapItemColor;
    TipDialog m_tipDialog;
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnUpload();
    afx_msg void OnNewDir();
    afx_msg void OnDownload();
    afx_msg void OnDelete();
    afx_msg void OnRename();
    afx_msg void OnCopy();
    afx_msg void OnCut();
    afx_msg void OnPaste();
    afx_msg void OnRefresh();
    afx_msg void OnOpen();
    afx_msg void OnInfo();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnEnKillfocusShow();
    void showTipMessage(const CString &s);
    afx_msg void OnSortName();
    afx_msg void OnSortDate();
    afx_msg void OnSortType();
    afx_msg void OnSortSize();
    afx_msg void OnSortAsc();
    afx_msg void OnSortDec();
    void pasteThread(const vector<CString>vec, const vector<CString>selectText, const CString oldPath, const CString newPath, const int option);
protected:
    afx_msg LRESULT OnGiveSelf(WPARAM wParam, LPARAM lParam);
};

