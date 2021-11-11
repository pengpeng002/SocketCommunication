#include "pch.h"
#include "CMListCtrl.h"
#include "MFCApplication5.h"
#include "afxdialogex.h"
#include "direct.h"
#include <stack>
#define GIVE_SELF 4013
enum {
	PASTE_OK, REFRESH
};

CMListCtrl::CMListCtrl()
{
	m_tipDialog.Create(IDD_PROPPAGE_SMALL);
	m_idSort = ID_32785;
	m_idCmp = ID_32789;
}

BEGIN_MESSAGE_MAP(CMListCtrl, CListCtrl)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CMListCtrl::OnNMCustomdraw)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CMListCtrl::OnNMRClick)
	ON_NOTIFY_REFLECT(NM_CLICK, &CMListCtrl::OnNMClick)
	ON_COMMAND(ID_UPLOAD, &CMListCtrl::OnUpload)
	ON_COMMAND(ID_NEW_DIR, &CMListCtrl::OnNewDir)
	ON_COMMAND(ID_DOWNLOAD, &CMListCtrl::OnDownload)
	ON_COMMAND(ID_DELETE, &CMListCtrl::OnDelete)
	ON_COMMAND(ID_RENAME, &CMListCtrl::OnRename)
	ON_COMMAND(ID_COPY, &CMListCtrl::OnCopy)
	ON_COMMAND(ID_CUT, &CMListCtrl::OnCut)
	ON_COMMAND(ID_PASTE, &CMListCtrl::OnPaste)
	ON_COMMAND(ID_REFRESH, &CMListCtrl::OnRefresh)
	ON_COMMAND(ID_OPEN, &CMListCtrl::OnOpen)
	ON_COMMAND(ID_INFO, &CMListCtrl::OnInfo)
	ON_WM_DROPFILES()
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CMListCtrl::OnNMDblclk)
	ON_EN_KILLFOCUS(IDC_EDIT1, &CMListCtrl::OnEnKillfocusShow)
	ON_COMMAND(ID_32785, &CMListCtrl::OnSortName)
	ON_COMMAND(ID_32786, &CMListCtrl::OnSortDate)
	ON_COMMAND(ID_32787, &CMListCtrl::OnSortType)
	ON_COMMAND(ID_32788, &CMListCtrl::OnSortSize)
	ON_COMMAND(ID_32789, &CMListCtrl::OnSortAsc)
	ON_COMMAND(ID_32790, &CMListCtrl::OnSortDec)
	ON_MESSAGE(GIVE_SELF, &CMListCtrl::OnGiveSelf)
END_MESSAGE_MAP()


void CMListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//printf("mouse move\n");
	static DWORD preItem = -1;
	LVHITTESTINFO ht;
	GetCursorPos(&(ht.pt));
	ScreenToClient(&ht.pt);
	HitTest(&ht);
	if (ht.iItem != -1 && ht.iItem != preItem)
	{
		//printf("item = %d\n", ht.iItem);
		TEXT_BK tb;
		tb.colText = RGB(0, 0, 0);
		tb.colTextBk = RGB(196, 220, 252);
		DWORD nowItem = ht.iItem;
		MapItemColor.SetAt(nowItem, tb);
		tb.colTextBk = RGB(252, 252, 252);
		if (preItem != -1)
		{
			MapItemColor.SetAt(preItem, tb);
			RedrawItems(min(preItem, nowItem), max(preItem, nowItem));
		}
		else
		{
			RedrawItems(nowItem, nowItem);
		}
		preItem = nowItem;
	}
	else if (ht.iItem == -1 && preItem != -1 )
	{
		TEXT_BK tb;
		tb.colText = RGB(0, 0, 0);
		tb.colTextBk = RGB(252, 252, 252);
		MapItemColor.SetAt(preItem, tb);
		RedrawItems(preItem, preItem);
		preItem = ht.iItem;
	}
	//else
	//{
	//	SYSTEMTIME sys;
	//	GetLocalTime(&sys);
	//	char str[50];
	//	sprintf(str, "%02d:%02d:%02d.%03d", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
	//	printf("now=%d, pre=%d:  %s\n", ht.iItem, preItem, str);
	//}
	
	CListCtrl::OnMouseMove(nFlags, point);
}


void CMListCtrl::OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = CDRF_DODEFAULT;
	NMLVCUSTOMDRAW* lplvdr = (NMLVCUSTOMDRAW*)pNMHDR;
	NMCUSTOMDRAW& nmcd = lplvdr->nmcd;
	switch (lplvdr->nmcd.dwDrawStage)//判断状态   
	{
		case CDDS_PREPAINT:
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		}
		case CDDS_ITEMPREPAINT://如果为画ITEM之前就要进行颜色的改变   
		{
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			break;
		}
		case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		{
			TEXT_BK tb; //printf("%d\n", lplvdr->iSubItem);
			lplvdr->clrTextBk = RGB(252, 252, 252);
			if (MapItemColor.Lookup(nmcd.dwItemSpec, tb))// tb =  MapItemColor[nmcd.dwItemSpec]
			{
				//lplvdr->clrText = tb.colText;
				lplvdr->clrTextBk = tb.colTextBk;
			}
			if (lplvdr->iSubItem != 0)
			{
				lplvdr->clrText = RGB(109, 109, 109);
			}
			*pResult = CDRF_DODEFAULT;
			break;
		}
	}
}


void CMListCtrl::OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	//printf("get a right click message\n");
	GetParent()->PostMessageA(LISTCLICK);
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));
	ScreenToClient(&point);
	//定义结构体   
	LVHITTESTINFO lvinfo;
	lvinfo.pt = point;
	//获取行号信息   
	int nItem = HitTest(&lvinfo);
	int m_itemSel = -1;
	if (nItem != -1)
		m_itemSel = lvinfo.iItem;   //当前行号

	if (lvinfo.flags == LVHT_ONITEMSTATEICON)
	{
		if (GetCheck(m_itemSel))
		{
			SetItemState(m_itemSel, 0, LVIS_SELECTED);
			SetSelectionMark(-1);
		}
		else
		{
			SetItemState(m_itemSel, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			SetSelectionMark(m_itemSel);
		}
	}
	else
	{
		for (int i = 0; i < GetItemCount(); i++)
		{
			SetCheck(i, GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED);
		}
		printf("get item!\n");
		int num = 0;
		POSITION pos = GetFirstSelectedItemPosition();
		while (pos)
		{
			num++;
			if (num > 1) break;
			GetNextSelectedItem(pos);
		}
		printf("get num!\n");
		CMenu menu;
		CMenu* popup;
		POINT pt = { 0 };
		GetCursorPos(&pt);
		if (num == 0)
		{
			menu.LoadMenuA(IDR_MENU2);
			popup = menu.GetSubMenu(0);
			if (option == -1)
			{
				popup->RemoveMenu(ID_PASTE, MF_BYCOMMAND);
			}
			printf("removed issort %d, idcmp %d\n", m_idSort, m_idCmp);
			CMenu* sortm = popup->GetSubMenu(5+(option != -1));
			sortm->CheckMenuRadioItem(ID_32785, ID_32788, m_idSort, MF_BYCOMMAND);
			sortm->CheckMenuRadioItem(ID_32789, ID_32790, m_idCmp, MF_BYCOMMAND);
		}
		else
		{
			menu.LoadMenuA(IDR_MENU3);
			popup = menu.GetSubMenu(0);
			if (1)
			{
				popup->RemoveMenu(ID_INFO, MF_BYCOMMAND);
			}
			if (option == -1)
			{
				popup->RemoveMenu(ID_PASTE, MF_BYCOMMAND);
			}
			if (num > 1)
			{
				popup->RemoveMenu(ID_OPEN, MF_BYCOMMAND);
				popup->RemoveMenu(ID_RENAME, MF_BYCOMMAND);
			}
		}
		printf("0000000000000000\n");
		popup->TrackPopupMenu(0, pt.x, pt.y, this);
		printf("selected num = %d\n", num);
	}
	

	*pResult = 0;
}


void CMListCtrl::OnNMClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	printf("get a message\n");
	GetParent()->PostMessageA(LISTCLICK);
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));
	ScreenToClient(&point);
	//定义结构体   
	LVHITTESTINFO lvinfo;
	lvinfo.pt = point;
	//获取行号信息   
	int nItem = HitTest(&lvinfo);
	int m_itemSel = -1;
	if (nItem != -1)
		m_itemSel = lvinfo.iItem;   //当前行号

	if (lvinfo.flags == LVHT_ONITEMSTATEICON)
	{
		if (GetCheck(m_itemSel))
		{
			SetItemState(m_itemSel, 0, LVIS_SELECTED);
			SetSelectionMark(-1);
		}
		else
		{
			SetItemState(m_itemSel, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			SetSelectionMark(m_itemSel);
		}
	}
	else
	{
		for (int i = 0; i < GetItemCount(); i++)
		{
			SetCheck(i, GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED);
		}
	}
	*pResult = 0;
}


void uploadFolder(const char* name)
{
	//const char* name = path + getPathName(path) + 1;
	CString&& cname = findFitName(name);
	g_newDir(cname.GetBuffer());
	CString temp = path;
	path = path + cname + "\\";
	typedef pair<CString, bool> pa;
	stack<pa> sta;
	sta.push(make_pair(name, 0));
	while (!sta.empty())
	{
		pa pp = sta.top();
		sta.pop();
		printf("file or folder name %s\n", pp.first.GetBuffer());
		if (pp.second)
		{
			//printf("filename %s\n", pp.first.GetBuffer());
			g_upload(pp.first.GetBuffer());
		}
		else
		{
			char temp[MAX_PATH + 5];
			sprintf(temp, "%s", pp.first.GetBuffer());
			temp[pp.first.GetLength()] = 0;
			int d = getPathName(temp);
			char* ss = temp + getPathName(temp) + 1;
			if (d == 0) ss--;
			printf("ss=%s d=%d\n", ss, d);
			if (pp.first != CString(name)) g_newDir(pp.first.GetBuffer());
			CString com = "dir " + g_uploadPath + (pp.first==CString(name)?"":pp.first) + " /b";
			printf("command %s\n", com.GetBuffer());
			char* line = new char[MAX_PATH + 5];
			FILE* fp = _popen(com.GetBuffer(), "r");
			while (fscanf(fp, "%[^\n]", line) > 0)
			{
				fscanf(fp, "%*c");
				CString temp = CString(pp.first == CString(name) ? "" : pp.first + "\\") + line;
				sta.push(make_pair(temp, isFile((g_uploadPath + temp).GetBuffer())));
			}
			_pclose(fp);
			delete[]line;
		}
	}
	path = temp;
	

}


void CMListCtrl::OnUpload()
{
	// TODO: 在此添加命令处理程序代码
	//char szBuffer[MAX_PATH] = { 0 };
	char* buf = new char[MAX_PATH];
	memset(buf, 0, sizeof(char) * MAX_PATH);
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = NULL;
	bi.pszDisplayName = buf;
	bi.lpszTitle = "从下面选择文件或文件夹:";
	bi.ulFlags = BIF_BROWSEINCLUDEFILES | BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (NULL == idl)
	{
		return;
	}
	SHGetPathFromIDList(idl, buf);

	updateVec(this);
	int len = getPathName(buf);
	g_uploadPath = CString(buf, len+1);

	if (isFile(buf))
	{
		g_upload(buf+len+1);
		//printf("send fksjdlfkjslkjflkasjflksjsalkjsdl\n");
	}
	else
	{
		g_uploadPath = CString(buf)+"\\";
		uploadFolder(buf+len+1);
	}
	delete[]buf;
	OnRefresh();
}


void CMListCtrl::OnNewDir()
{
	// TODO: 在此添加命令处理程序代码
	updateVec(this);
	g_newDir();
	OnRefresh();
	int count = GetItemCount();
	SetItemState(count-1, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	SetSelectionMark(count-1);
	OnRename();
}


void CMListCtrl::OnDownload()
{
	// TODO: 在此添加命令处理程序代码
	g_tempSavePath = g_savePath;
	vector<int> vei;
	POSITION pos = GetFirstSelectedItemPosition();//查找所有选中
	while (pos)
	{
		int ind = GetNextSelectedItem(pos);
		vei.push_back(ind);
	}

	if (vei.size() > 1 || GetItemText(GetSelectionMark(), 1) == "文件夹")//判断是否允许下载
	{
		MessageBox("抱歉，暂时只支持单个文件下载");
		return;
	}

	if (!g_useDefaultPath)//交由用户选择下载路径
	{
		CFolderPickerDialog fpd;
		fpd.m_ofn.lpstrTitle = "请选择下载路径";
		if (fpd.DoModal() == TRUE)
		{
			g_tempSavePath = fpd.GetFolderPath()+"\\";
		}
	}

	string com = "dir " + string(g_tempSavePath.GetBuffer()) + " /b";
	printf("command : %s\n", com.c_str());
	FILE* fp = _popen(com.c_str(), "r");//以读的形式打开管道，获取dir命令执行结果 
	char* line = new char[MAX_PATH + 5];//一行的长度，即文件名最大长度
	vec.clear();
	if (fp)
	{
		while (fscanf(fp, "%[^\n]", line) > 0)//读取一行 
		{
			fscanf(fp, "%*c");
			vec.push_back(CString(line));
		}
	}

	for (auto i : vei)
	{
		CString str = GetItemText(i, 0);
		bool isFolder = GetItemText(i, 1) == "文件夹";
		if (!isFolder)
		{
			char* cstr = new char[str.GetLength() + 2];
			sprintf(cstr, "%s", str.GetBuffer());
			cstr[str.GetLength()] = 0;
			printf("cstr=%s\n", cstr);
			g_download(str.GetBuffer());
			delete[]cstr;
		}
		else
		{
			CString saveName = findFitName(str.GetBuffer());
			_mkdir(CString(g_tempSavePath + saveName).GetBuffer());
			CString temp = g_tempSavePath;
			g_tempSavePath += saveName + "\\";
			typedef pair<CString, bool> pa;
			stack<pa> sta;
			sta.push(make_pair(str, 1));
			while (!sta.empty())
			{
				pa pp = sta.top();
				sta.pop();
				//printf("sta.top.first = %s\n", pp.first.GetBuffer());
				if (pp.second == 1)
				{
					g_refresh(path + pp.first + "\\");
					for (int i = showInfoLen - 1; i >= 0; i--)
					{
						sta.push(make_pair(pp.first+"\\"+showInfo[i].fileName, showInfo[i].isFolder == "文件夹"));
					}
					if (pp.first == str) continue;
					//else printf("%s %s\n", pp.first.GetBuffer(), saveName.GetBuffer());
					CString out = g_tempSavePath + CString(pp.first.GetBuffer() + str.GetLength() + 1);
					printf("out=%s\n", out.GetBuffer());
					//printf("path=%s, first=%s, savepath = %s\n", path.GetBuffer(), pp.first.GetBuffer(), g_tempSavePath.GetBuffer());
					_mkdir(out.GetBuffer());
				}
				else
				{
					//printf("download path = %s\n", pp.first.GetBuffer() + str.GetLength() + 1);
					CString temp = path;
					path += str + "\\";
					g_download(pp.first.GetBuffer() + str.GetLength() + 1);
					path = temp;
				}
			}
			g_tempSavePath = temp;
		}
	}
	
	
}


void CMListCtrl::OnDelete()
{
	// TODO: 在此添加命令处理程序代码
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos)
	{
		int index = GetNextSelectedItem(pos);
		CString str = GetItemText(index, 0);
		g_delete(str);
	}
	OnRefresh();
}


void CMListCtrl::OnRename()
{
	// TODO: 在此添加命令处理程序代码
	//return;
	POSITION pos = GetFirstSelectedItemPosition();
	m_index = GetNextSelectedItem(pos);
	CRect rect;
	GetSubItemRect(m_index, 0, LVIR_LABEL, rect);
	oldName = GetItemText(m_index, 0);
	printf("to rename\n");
	m_edit = new CEdit();
	m_edit->Create(ES_AUTOHSCROLL | WS_CHILD | ES_LEFT | ES_WANTRETURN | WS_BORDER, CRect(0, 0, 0, 0), this, IDC_EDIT1);
	m_edit->SetWindowText(oldName);//将子项的内容显示到编辑框中
	m_edit->ShowWindow(SW_SHOW);//显示编辑框
	m_edit->MoveWindow(&rect);//将编辑框移动到子项上面，覆盖在子项上
	m_edit->SetFocus();//使编辑框取得焦点
	m_edit->SetSel(0,-1);//使光标移到最后面
	//g_rename();
}


void CMListCtrl::OnCopy()
{
	// TODO: 在此添加命令处理程序代码
	//g_copy();
	//return;
	POSITION pos = GetFirstSelectedItemPosition();
	selectText.clear();
	while (pos)
	{
		int index = GetNextSelectedItem(pos);
		CString str = GetItemText(index, 0);
		selectText.push_back(str);
	}
	option = 1;//binary   01
	oldPath = path;
	
}


void CMListCtrl::OnCut()
{
	// TODO: 在此添加命令处理程序代码
	//g_cut();
	//return;
	POSITION pos = GetFirstSelectedItemPosition();
	selectText.clear();
	while (pos)
	{
		int index = GetNextSelectedItem(pos);
		CString str = GetItemText(index, 0);
		selectText.push_back(str);
	}
	option = 0;//binary   00
	oldPath = path;
}

void CMListCtrl::pasteThread(const vector<CString>vec, const vector<CString>selectText, const CString oldPath, const CString newPath, const int option)
{
	SOCKET Client = socket(AF_INET, SOCK_STREAM, 0);//TCP通信
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4011);
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.252");
	if (connect(Client, (sockaddr*)&addr, sizeof(addr)))
	{
		printf("connect error, error code: %d\n", WSAGetLastError());
		return;
	}
	for (auto i : selectText)
	{
		CString oldName = i;
		CString newName = findFitName(oldName.GetBuffer(), &vec);
		g_paste(Client, oldPath + oldName, newPath + newName, option);
		printf("now paste file %s\n", i.GetBuffer());
	}
	PostMessageA(GIVE_SELF, PASTE_OK);
	//showTipMessage("文件粘贴成功");
	if (path == newPath)
	{
		PostMessageA(GIVE_SELF, REFRESH);
		//g_refresh();
		//g_showList(this);
	}
	int opt = 0;
	send(Client, (char*)&opt, 4, 0);
}

void CMListCtrl::OnPaste()
{
	// TODO: 在此添加命令处理程序代码
	//return;
	int lp = path.GetLength(), lop = oldPath.GetLength();
	if (lp >= lop)
	{
		for (auto i : selectText)
		{
			CString s1 = oldPath + i;
			if (path.GetLength() >= s1.GetLength() && path.Left(s1.GetLength()) == s1)
			{
				if (option & 1)
				{
					showTipMessage("不能将文件复制到自身或其子目录下");
				}
				else
				{
					showTipMessage("不能将文件移动到自身或其子目录下");
				}
				return;
			}
		}
	}
	updateVec(this);
	//pasteThread(vec, selectText, oldPath, path, option);
	std::thread th(&CMListCtrl::pasteThread, this, vec, selectText, oldPath, path, option);
	th.detach();
	
	//for (auto i : selectText)
	//{
	//	oldName = i;
	//	
	//	g_paste();
	//}
	if (option == 0) option = -1;
	
}


void CMListCtrl::OnRefresh()
{
	// TODO: 在此添加命令处理程序代码
	printf("now is to refresh!\n");
	g_refresh();
	g_showList(this);
	GetParent()->PostMessageA(LISTCLICK);
}


void CMListCtrl::OnOpen()
{
	// TODO: 在此添加命令处理程序代码
	int index = GetSelectionMark();
	CString name = GetItemText(index, 0);
	if (GetItemText(index, 1) == "文件夹")
	{
		path += name + "\\";
		lastPath = path;
		goforward = 0;
		goback = 1;
	}
	OnRefresh();
}


void CMListCtrl::OnInfo()
{
	// TODO: 在此添加命令处理程序代码
	g_info();
}


void CMListCtrl::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	char* name = new char[MAX_PATH + 5];
	//CString name;
	int num = DragQueryFile(hDropInfo, -1, 0, 0);
	updateVec(this);
	for (int i=0; i<num; i++)
	//if (num)
	{
		//char name[MAX_PATH + 5];
		DragQueryFileA(hDropInfo, i, name, MAX_PATH);
		int len = getPathName(name);
		g_uploadPath = CString(name, len + 1);
		if (isFile(name))
		{
			g_upload(name+len+1);
		}
		else
		{
			g_uploadPath = CString(name)+"\\";
			uploadFolder(name+len+1);
			
		}
		//name[strlen(name)] = 0;
		
		//
	}
	//上传
	
	//上传完成后的收尾
	DragFinish(hDropInfo);
	g_refresh();
	g_showList(this);
	delete[]name;
	CListCtrl::OnDropFiles(hDropInfo);
}


void CMListCtrl::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	OnOpen();
	*pResult = 0;
}

void CMListCtrl::OnEnKillfocusShow()
{
	m_edit->GetWindowTextA(newName);
	SetItemText(m_index, 0, newName.GetBuffer());
	m_edit->ShowWindow(SW_HIDE);
	delete m_edit;
	m_edit = NULL;
	g_rename();
}

void CMListCtrl::showTipMessage(const CString& s)
{
	m_tipDialog.setTipText(s);
	CRect rect, rectm;
	m_tipDialog.GetWindowRect(&rect);
	GetWindowRect(&rectm);
	m_tipDialog.MoveWindow(rectm.left + 280, rectm.top, rect.Width(), rect.Height());
	m_tipDialog.ShowWindow(SW_SHOW);
}

void CMListCtrl::OnSortName()
{
	// TODO: 在此添加命令处理程序代码
	m_idSort = ID_32785;
	sortColumn = 0;
	SortItems(CompareFunc, (DWORD_PTR)this);
}

void CMListCtrl::OnSortDate()
{
	// TODO: 在此添加命令处理程序代码
	m_idSort = ID_32786;
	sortColumn = 2;
	SortItems(CompareFunc, (DWORD_PTR)this);
}

void CMListCtrl::OnSortType()
{
	// TODO: 在此添加命令处理程序代码
	m_idSort = ID_32787;
	sortColumn = 1;
	SortItems(CompareFunc, (DWORD_PTR)this);
}

void CMListCtrl::OnSortSize()
{
	// TODO: 在此添加命令处理程序代码
	m_idSort = ID_32788;
	sortColumn = 3;
	SortItems(CompareFunc, (DWORD_PTR)this);
}

void CMListCtrl::OnSortAsc()
{
	// TODO: 在此添加命令处理程序代码
	m_idCmp = ID_32789;
	isDesc = 0;
	SortItems(CompareFunc, (DWORD_PTR)this);
}

void CMListCtrl::OnSortDec()
{
	// TODO: 在此添加命令处理程序代码
	m_idCmp = ID_32790;
	isDesc = 1;
	SortItems(CompareFunc, (DWORD_PTR)this);
}


afx_msg LRESULT CMListCtrl::OnGiveSelf(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case PASTE_OK: {
		showTipMessage("文件粘贴成功");
	}
	case REFRESH: {
		OnRefresh();
	}
	}
	return 0;
}
