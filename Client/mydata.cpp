#include "pch.h"
#include "mydata.h"
#include "windows.h"
#include "stdio.h"
#include "io.h"
#include "direct.h"

char name[20]="null", pass[30]="null";
CString g_savePath = "E:\\ClientDownload\\";
CString oldName, newName;
CString oldPath;
CString lastPath="\\\\";
CString g_uploadPath;
int option=-1;
int showInfoLen = 0;
int sortColumn = 0;
bool isDesc = 1;
bool goforward = 0, goback = 0;
const int bufferSize = 1024*1024*11;//11
char buffer[bufferSize];
char defaultPath[MAX_PATH];
bool g_autoRun=0, g_useBubble=0, g_useDefaultPath=1;
ShowInfo* showInfo;
vector<CString> vec, selectText;
//CString findFitName(const char* str);
enum {
	EXIT, LOGIN, UPLOAD, DOWNLOAD, REFRESH, DELETED, NEWDIR, RENAME, PASTE
};

void updateVec(CListCtrl* m_fileList)
{
	vec.clear();
	for (int i = 0; i < m_fileList->GetItemCount(); i++)
	{
		vec.push_back(m_fileList->GetItemText(i, 0));
	}
}

bool checkPassword(char* name, char* pass)
{
	int nameLen, passLen;
	nameLen = strlen(name);
	passLen = strlen(pass);
	int opt = LOGIN;
	send(Client, (char*)&opt, 4, 0);
	send(Client, (char*)&nameLen, 4, 0);
	send(Client, (char*)&passLen, 4, 0);
	send(Client, name, nameLen, 0);
	send(Client, pass, passLen, 0);
	bool ret;
	recv(Client, (char*)&ret, 1, 0);
	return ret;
}

void g_showList(CListCtrl* m_fileList)
{
	m_fileList->DeleteAllItems();
	CImageList* imageList = m_fileList->GetImageList(LVSIL_SMALL);
	for (int i = 0; i < showInfoLen; i++)
	{
		string type="文件", icon;
		SHFILEINFO info;
		int iicon = -1;
		if (SHGetFileInfo(showInfo[i].fileName, (showInfo[i].isFolder=="文件夹")? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL, &info, sizeof(info), SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME))
		{
			iicon = imageList->Add(info.hIcon);
			type = info.szTypeName;
		}
		m_fileList->InsertItem(i, showInfo[i].fileName, iicon);
		m_fileList->SetItemText(i, 1, (showInfo[i].isFolder == "文件夹")?"文件夹":type.c_str());
		m_fileList->SetItemText(i, 2, showInfo[i].fileTime);
		if (showInfo[i].isFolder != "文件夹")
		{
			m_fileList->SetItemText(i, 3, showInfo[i].fileSize);
		}
		m_fileList->SetItemData(i, i);
	}
	delete[]showInfo;
	showInfoLen = 0;
}

long long s2i(const char* str)
{
	long long x = 0;;
	int len = strlen(str);
	for (int i = 0; i < len; i++)
	{
		if (str[i] == ',');
		else if (str[i] == ' ') break;
		else if (str[i] >= '0' && str[i] <= '9')
		{
			x = x * 10 + (str[i] - '0');
		}
	}
	return x;
}

void init()
{
	FILE* fp = fopen("data.dat", "rb");
	
	if (fp)
	{
		fread(&g_auto, 1, 1, fp);
		fread(&g_remember, 1, 1, fp);
		fread(&g_autoRun, 1, 1, fp);
		fread(&g_useBubble, 1, 1, fp);
		fread(&g_useDefaultPath, 1, 1, fp);
		int len=0;

		fread(&len, 4, 1, fp);
		fread(name, len, 1, fp);

		fread(&len, 4, 1, fp);
		fread(pass, len, 1, fp);

		fread(&len, 4, 1, fp);
		printf("read len = %d\n", len);

		g_savePath.Preallocate(len + 5);
		char* str = g_savePath.GetBuffer();
		fread(str, len, 1, fp);
		str[len] = 0;


		if (g_auto)
		{
			bool ret = checkPassword(name, pass);
			if (ret)
			{
				g_isLogin = 1;
			}
		}

	}
}

void save()
{
	FILE* fp = fopen("data.dat", "wb");
	if (fp)
	{
		fwrite(&g_auto, 1, 1, fp);
		fwrite(&g_remember, 1, 1, fp);
		fwrite(&g_autoRun, 1, 1, fp);
		fwrite(&g_useBubble, 1, 1, fp);
		fwrite(&g_useDefaultPath, 1, 1, fp);
		int len;
		len = strlen(name);
		fwrite(&len, 4, 1, fp);
		fwrite(name, len, 1, fp);
		len = strlen(pass);
		fwrite(&len, 4, 1, fp);
		fwrite(pass, len, 1, fp);
		printf("name = %s, pass = %s\n", name, pass);
		len = g_savePath.GetLength();
		fwrite(&len, 4, 1, fp);
		fwrite(g_savePath.GetBuffer(), len, 1, fp);
	}
}

int getPathName(const char* path)
{
	int len = strlen(path);
	int st = 0, ed = 0;
	for (int i = len - 2; i >= 0; i--)
	{
		if (ed == 0 && path[i] == '.')
		{
			ed = i;
		}
		else if (st == 0 && path[i] == '\\')
		{
			st = i;
			break;
		}
	}
	return st;
}

void g_upload(char *str/* = NULL*/)
{
	if (str == NULL)
	{
		CFileDialog openDlg(1, "txt", 0, 6UL, "所有文件 (*.*)|*.*||");
		int ret = openDlg.DoModal();
		if (ret == IDOK)
		{
			str = new char[MAX_PATH + 5];
			sprintf(str, "%s", openDlg.GetPathName().GetBuffer());
		}
		else
		{
			return;
		}
	}
	//printf("file path : %s 0x%p\n", str, str);
	//int len = getPathName(str);
	CString&& name = findFitName(str);
	CString fi;
	fi.Format("%s%s", path.GetBuffer(), name.GetBuffer());
	printf("file path : %s\n", fi.GetBuffer());//存到服务器的绝对路径
	printf("local path: %s\n", (g_uploadPath + str).GetBuffer());
	int opt = UPLOAD;//modify
	int len=fi.GetLength();
	send(Client, (char*)&opt, 4, 0);//UPLOAD
	send(Client, (char*)&len, 4, 0);//name len
	send(Client, fi.GetBuffer(), len, 0);//name
	FILE* fp = fopen((g_uploadPath+str).GetBuffer(), "rb");
	if (!fp)
	{
		printf("can not open!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	}
	struct _stati64 info;
	int retx = _stati64((g_uploadPath + str).GetBuffer(), &info);//获取文件信息
	long long fileLen = info.st_size;
	//if (fileLen > INT_MAX || fileLen < 0)
	//{
	//	fileLen = 0;
	//	MessageBox(0, "文件最大限制为2GB", "error", 0);
	//}
	send(Client, (char*)&fileLen, 8, 0);//file len
	printf("file len = %lld, retx = %d\n", fileLen, retx);
	fseek(fp, 0, SEEK_SET);
	long long sendLen = 0;
	clock_t st = clock();
	while (sendLen < fileLen && !feof(fp))
	{
		int readLen = fread(buffer, 1, bufferSize, fp);
		send(Client, buffer, readLen, 0);
		sendLen += readLen;
		printf("\r%02.2lf %lld / %lld", sendLen * 100.0f / fileLen, sendLen, fileLen);
	}
	printf("\nupload use time: %d\n", clock() - st);
	fclose(fp);
	printf("\nsend end\n");
	//delete[] str;
}

void g_newDir(const char* str/* ="新建文件夹" */)
{
	printf("new dir\n");
	int opt = NEWDIR;
	send(Client, (char*)&opt, 4, 0);
	CString&& name = findFitName(str);
	printf("new dir name %s %s\n", name.GetBuffer(), str);
	CString sp = path + name;
	int nameLen = sp.GetLength();
	send(Client, (char*)&nameLen, 4, 0);
	send(Client, sp.GetBuffer(), nameLen, 0);
}

void g_delete(CString &str)
{
	int opt = DELETED;
	str = path + str;
	int nameLen = str.GetLength();
	send(Client, (char*)&opt, 4, 0);//发送下载命令
	send(Client, (char*)&nameLen, 4, 0);//发送路径长度
	send(Client, str.GetBuffer(), nameLen, 0);//发送路径

}

void g_download(const CString name, const CString savePath, const SOCKET &Client)
{
	int opt = DOWNLOAD;
	int nameLen = name.GetLength();
	send(Client, (char*)&opt, 4, 0);//发送下载命令
	send(Client, (char*)&nameLen, 4, 0);//发送路径长度
	send(Client, name.GetString(), nameLen, 0);//发送路径
	long long fileLen = 0;
	recv(Client, (char*)&fileLen, 8, 0);//收到文件长度
	if (fileLen > INT_MAX || fileLen < 0)
	{
		fileLen = 0;
		MessageBox(0, "文件最大限制为2GB", "error", 0);
		//return;
	}
	printf("nameLen=%d, name=%s, fileLen=%lld, savePath=%s\n", nameLen, name.GetString(), fileLen, savePath.GetString());
	FILE* fp = fopen(savePath.GetString(), "wb");//打开文件
	long long saveLen = 0;
	clock_t st = clock();
	while (saveLen < fileLen)
	{
		int readLen = recv(Client, buffer, min(bufferSize, fileLen - saveLen), 0);
		fwrite(buffer, readLen, 1, fp);
		saveLen += readLen;
		printf("\r%02.2lf %lld / %lld", saveLen * 100.0f / fileLen, saveLen, fileLen);
	}
	printf("\ndownload use time: %d\n", clock() - st);
	printf("\nsave end\n");
	fclose(fp);
}

void g_rename()
{
	if (oldName == newName) return;
	int opt = RENAME;
	int oldLen = oldName.GetLength(), newLen = newName.GetLength(), pathLen = path.GetLength();
	send(Client, (char*)&opt, 4, 0);//发送下载命令
	send(Client, (char*)&pathLen, 4, 0);
	send(Client, path.GetBuffer(), pathLen, 0);

	send(Client, (char*)&oldLen, 4, 0);
	send(Client, oldName.GetBuffer(), oldLen, 0);

	send(Client, (char*)&newLen, 4, 0);
	send(Client, newName.GetBuffer(), newLen, 0);


}

void g_copy()
{

}

void g_cut()
{

}

void g_paste(const SOCKET &Client, const CString path_old, const CString path_new, const int option)
{
	//if (oldPath == path && !(option & 1)) return;
	//newName = findFitName(oldName.GetBuffer());
	//CString path_old = oldPath + oldName;
	//CString path_new = path + newName;
	int len_old = path_old.GetLength(), len_new = path_new.GetLength();
	printf("oldpath=%s, newpath = %s\n", path_old.GetString(), path_new.GetString());
	int opt = PASTE;
	//return;
	send(Client, (char*)&opt, 4, 0);
	send(Client, (char*)&len_old, 4, 0);
	send(Client, path_old.GetString(), len_old, 0);
	send(Client, (char*)&len_new, 4, 0);
	send(Client, path_new.GetString(), len_new, 0);
	send(Client, (char*)&option, 4, 0);
	recv(Client, (char*)&opt, 4, 0);
}

CString intAddComma(long long x)
{
	CString str;
	str.Format("%d", x);
	for (int i = str.GetLength() - 3; i > 0; i -= 3)
	{
		str.Insert(i, ',');
	}
	return move(str);
}

void g_refresh(CString path/* = "null"*/)
{
	if (path == "null")
	{
		path = ::path;
	}
	int opt = REFRESH;
	int sendLen = send(Client, (char*)&opt, 4, 0);
	int len = path.GetLength();
	sendLen = send(Client, (char*)&len, 4, 0);
	sendLen = send(Client, path.GetBuffer(), len, 0);
	recv(Client, (char*)&len, 4, 0);
	FileInfo fileInfo;
	showInfo = new ShowInfo[len + 1];
	showInfoLen = len;
	for (int i = 0; i < len; i++)
	{
		int len;
		recv(Client, (char*)&len, 4, 0);
		char* name = new char[len + 2];
		recv(Client, name, len, 0);
		name[len] = 0;
		recv(Client, (char*)&fileInfo, sizeof(FileInfo), 0);
		//printf("filename: %s\n", name);
		//printf("isFolder = %d  ", fileInfo.isFolder);
		//printf("info.st_size = %d  ", fileInfo.fileSize);
		int fileTime = fileInfo.fileTime;
		int y, m, d, h, f;
		y = fileTime >> 24;
		m = (fileTime >> 18) & 0x3f;
		d = (fileTime >> 12) & 0x3f;
		h = (fileTime >> 6) & 0x3f;
		f = (fileTime) & 0x3f;
		showInfo[i].fileName = CString(name);
		showInfo[i].fileSize = intAddComma(ceil(fileInfo.fileSize * 1.0 / 1024)) + " KB";
		showInfo[i].isFolder = fileInfo.isFolder ? "文件夹" : "文件";
		showInfo[i].fileTime.Format("%04d/%02d/%02d %02d:%02d\n", y+1900, m, d, h, f);
	}
}

void g_open()
{

}

void g_info()
{

}

bool findHave(CString& str, const vector<CString>* vec)
{
	for (auto i : *vec)
	{
		if (i == str)
		{
			return 1;
		}
	}
	return 0;
}

CString findFitName(const char* str, const vector<CString>*vec/*= NULL*/)
{
	if (vec == NULL) vec = &::vec;
	int len = strlen(str);
	int pos = 0;
	for (int i = len - 1; i > 0; i--)
	{
		if (str[i] == '.')
		{
			pos = i;
			break;
		}
	}
	char* ss = new char[len+2];
	CString name, ext;
	name.Format("%s", str);
	if (pos != 0)
	{
		memcpy(ss, str, pos);
		ss[pos] = 0;
		ext = CString(str + pos);
	}
	else
	{
		memcpy(ss, str, len);
		ss[len] = 0;
		ext = "";
	}
	if (findHave(name, vec))
	{
		int add = 1;
		while (1)
		{
			name.Format("%s-%d%s", ss, add, ext.GetBuffer());
			add++;
			if (!findHave(name, vec)) break;
		}
	}
	delete[]ss;
	return move(name);
}


void bootRun(bool cancel/*=false*/)
{
	const char* str = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	const char* regName = "ClientAutoRun";
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, str, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		if (cancel)
		{
			RegDeleteValue(hKey, regName);
			RegCloseKey(hKey);
		}
		else
		{
			char pFileName[MAX_PATH] = { 0 };
			DWORD dwRet = GetModuleFileName(NULL, pFileName, MAX_PATH);
			RegSetValueEx(hKey, regName, 0, REG_SZ, (BYTE*)pFileName, dwRet);
			RegCloseKey(hKey);
		}
	}
}

int CALLBACK CompareFunc(LPARAM   lParam1, LPARAM   lParam2, LPARAM   lParamSort)
{
	CListCtrl* pListCtrl = (CListCtrl*)lParamSort;//{这里面都是固定语法,适当了解
	LVFINDINFO findInfo;
	findInfo.flags = LVFI_PARAM;
	findInfo.lParam = lParam1;
	int iItem1 = pListCtrl->FindItem(&findInfo, -1);
	findInfo.lParam = lParam2;
	int iItem2 = pListCtrl->FindItem(&findInfo, -1);//这里面都是固定语法,适当了解}

	CString strItem1 = pListCtrl->GetItemText(iItem1, sortColumn);   //需要研究
	CString strItem2 = pListCtrl->GetItemText(iItem2, sortColumn);   //需要研究
	char* str1 = (char*)strItem1.GetBuffer(strItem1.GetLength());  //需要研究
	char* str2 = (char*)strItem2.GetBuffer(strItem1.GetLength());  //需要研究

	bool f1 = (pListCtrl->GetItemText(iItem1, 1) == "文件夹");
	bool f2 = (pListCtrl->GetItemText(iItem2, 1) == "文件夹");
	if (sortColumn < 2)
	{
		if (f1 && !f2) return 0;
		else if (f2 && !f1) return 1;
	}
	if (sortColumn == 3)
	{
		if (isDesc) return s2i(strItem1) < s2i(strItem2);
		else return s2i(strItem1) > s2i(strItem2);
	}

	if (!isDesc) return strItem1.CompareNoCase(strItem2);
	else return strItem2.CompareNoCase(strItem1);


}


bool isFile(const char* str)
{
	FILE* fp = fopen(str, "rb");
	if (fp)
	{
		fclose(fp);
		return true;
	}
	return false;
}