#pragma once
#include "MFCApplication5.h"
#include <string>
#include <thread>
#include <vector>
using namespace std;
extern char name[20], pass[30];
struct FileInfo {
	long long fileSize;
	int fileTime;
	bool isFolder;
};
struct ShowInfo {
	CString fileName;
	CString isFolder;
	CString fileSize;
	CString fileTime;
};
extern ShowInfo* showInfo;
extern int showInfoLen;
extern CString oldName, newName;
extern CString oldPath;
extern CString lastPath;
extern CString g_savePath, g_tempSavePath;
extern CString g_uploadPath;
extern bool goforward, goback;
extern bool g_autoRun, g_useBubble, g_useDefaultPath;
extern char defaultPath[MAX_PATH];
extern int option;
extern int sortColumn;
extern bool isDesc;
extern vector<CString> vec, selectText;
void init();
void save();
void updateVec(CListCtrl* m_fileList);
bool checkPassword(char* name, char* pass);
int getPathName(const char* path);
void g_upload(char* str=NULL);
void g_newDir(const char* str="新建文件夹");
void g_delete(CString &str);
void g_download(char* str);
void g_rename();
void g_copy();
void g_cut();
void g_paste(const SOCKET &Client, const CString path_old, const CString path_new, const int option);
void g_refresh(CString path = "null");
void g_open();
void g_info();
void g_showList(CListCtrl* m_fileList);
void bootRun(bool cancel=false);
bool isFile(const char* str);
long long s2i(const char* str);
int CALLBACK CompareFunc(LPARAM   lParam1, LPARAM   lParam2, LPARAM   lParamSort);
CString findFitName(const char* str, const vector<CString>* vec = NULL);