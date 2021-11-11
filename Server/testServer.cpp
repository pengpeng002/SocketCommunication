#include <bits/stdc++.h>
#include "winsock2.h"
#include <windows.h>
#include "pengpeng.h"
using namespace std;
const char* showFilePath = "E:\\testDisk";//展示给用户的文件的存储路径，即客户端显示目录的根路径，本来应该根据权限不同，路径不同，但暂时不考虑权限 
SOCKET Client;
//const int bufferSize = 1024000;
//char buffer[bufferSize];
enum {//服务器可接受的所有指令，根据指令不同，执行的动作不同 
	EXIT, LOGIN, UPLOAD, DOWNLOAD, REFRESH, DELETED, NEWDIR, RENAME, PASTE
};
bool checkPassword(char* name, char* pass)//检查用户名密码是否正确 
{
	return strcmp(name, "admin")==0 && strcmp(pass, "admin")==0;
}
struct FileInfo {//发给客户端的文件列表的数据 
	long long fileSize;//文件大小 
	int fileTime;//文件修改时间 
	bool isFolder;//是否是文件夹 
};
int getPathName(const char* path)//根据路径获取文件名 返回最后一个\\ 在字符串中的位置 
{
	int len = strlen(path);
	int st = 0, ed = 0;
	for (int i = len - 1; i >= 0; i--)
	{
		if (ed == 0 && path[i] == '.')
		{
			ed = i;//没有意义 
		}
		else if (st == 0 && path[i] == '\\')
		{
			st = i;
			break;
		}
	}
	return st;
}

void getThread(SOCKET Client)//接受客户端指令的线程 
{
	const int bufferSize = 1024*1024*20;
	char* buffer = new char[bufferSize]; //文件传输的缓冲区 
	while(1)
	{
		int opt;
		printf("wait recv\n");
		int len = recv(Client, (char*)&opt, 4, 0);//接受到的指令 
		printf("len = %d, get opt = %d\n", len, opt);
		if (len <= 0) break;//客户端非正常退出，则接受不到断开连接指令 
		
		switch (opt)
		{
			case EXIT: {//客户端希望与服务器断开连接  客户端正常退出 
				closesocket(Client);//关闭套接字 
				delete[]buffer;//释放缓冲区，避免内存泄漏 
				return ;
				break;
			}
			case LOGIN: {//客户端登录 
				int nameLen, passLen;
				recv(Client, (char*)&nameLen, 4, 0);//用户名长度 
				recv(Client, (char*)&passLen, 4, 0);//密码长度 
				char *name = new char[nameLen+2];//开辟用户名数组 
				char* pass = new char[passLen+2];//开辟密码数组 
				recv(Client, name, nameLen, 0);//接受用户名 
				recv(Client, pass, passLen, 0);//接受密码 
				name[nameLen]=0, pass[passLen]=0;//手动给字符串尾部添加\0 
				bool ans = checkPassword(name, pass);//检查是否合法用户 
				send(Client, (char*)&ans, 1, 0);//告诉客户端检查结果 
				delete[]name;//释放申请的空间，避免内存泄露 
				delete[]pass;
				break;
			}
			
			case UPLOAD: {//客户端请求上传文件   仅单文件上传 
				int nameLen;
				long long fileLen;
				recv(Client, (char*)&nameLen, 4, 0);//文件名或文件路径 长度 
				char *path = new char[nameLen+5];//开辟文件名数组 
				recv(Client, path, nameLen, 0);//接受文件名。 注：客户端发过来的路径为客户端所认为的绝对路径，一定以\\\\开头 
				path[nameLen]=0;
				recv(Client, (char*)&fileLen, 8, 0);//文件长度 
				if (fileLen == 0)
				{
					delete[]path;
					break;
				}
				string savePath = string(showFilePath)+string(path+1);	//保存到服务器的路径，这里将客户端的绝对路径转化成了真正的服务器上的绝对路径 
				FILE *fp = fopen(savePath.c_str(), "wb");//以写的形式打开文件 
//				if (!fp) printf("error!!\n");//没有意义 
				long long saveLen = 0;//总共接收到的数据长度 
				while (saveLen < fileLen)
				{
					int readLen = recv(Client, buffer, min((long long)bufferSize, fileLen-saveLen), 0);//接收数据 
					fwrite(buffer, readLen, 1, fp);//将接收到的数据写入文件 
					saveLen += readLen;//更新总接收长度 
					printf("\r%02.2lf %lld / %lld", saveLen * 100.0f / fileLen, saveLen, fileLen);//打印文件接收进度 
				}
				printf("\nsave end\n");
				fclose(fp);//关闭文件缓冲区，真正将文件写入硬盘 
				delete[]path;//释放控件，避免内存泄露 
				break;
			}
			
			case DOWNLOAD: {//客户端请求下载文件   仅单文件下载 
				int nameLen;
				recv(Client, (char*)&nameLen, 4, 0);//文件名或文件路径 长度 
				char *path = new char[nameLen+5];//开辟文件名数组 
				recv(Client, path, nameLen, 0);//接受文件名。 注：客户端发过来的路径为客户端所认为的绝对路径，一定以\\\\开头 
				path[nameLen]=0;
				string str = string(showFilePath)+string(path+1);//在服务器上保存的路径   这里将客户端的绝对路径转化成了真正的服务器上的绝对路径
				struct _stati64 info;
				_stati64(str.c_str(), &info);//获取文件信息 
				long long len = info.st_size;
				FILE* fp = fopen(str.c_str(), "rb");//打开文件 
				send(Client, (char*)&len, 8, 0);//发送文件长度 
				if (len > INT_MAX) len = 0;
				fseek(fp, 0, SEEK_SET);//文件指针移到文件开头
				long long sendLen = 0;//已发送的数据长度 
				while (sendLen < len && !feof(fp))
				{
					int readLen = fread(buffer, 1, bufferSize, fp);//读取文件数据 
					send(Client, buffer, readLen, 0);//发送数据 
					sendLen += readLen;//更新已发送长度 
					printf("\r%02.2lf %lld / %lld", sendLen * 100.0f / len, sendLen, len);//打印文件发送进度 
				}
				fclose(fp);//关闭文件指针 
				delete[]path;//释放控件，避免内存泄露 
				printf("\nsend end\n");
				break;
			}
			
			case REFRESH: {//刷新显示 
				int len;
				recv(Client, (char*)&len, 4, 0);//需刷新显示的路径的长度 
				char *name = new char[len+2];
				recv(Client, name, len, 0);//刷新显示的路径 
				name[len]=0;				
				string com = "dir "+string(showFilePath)+string(name+1)+" /b";//用管道获取需刷新路径下的所有文件   不包括隐藏文件、系统文件等 
				printf("command : %s\n", com.c_str());
				FILE* fp = _popen(com.c_str(), "r");//以读的形式打开管道，获取dir命令执行结果 
				char *line = new char[MAX_PATH+5];//一行的长度，即文件名最大长度 
				vector<FileInfo> ve;//文件信息数组 
				vector<string> vec;//文件名数组，最开始设计时忘了文件名，只能这样加了 
				if (fp)
				{
					while(fscanf(fp, "%[^\n]", line)>0)//读取一行 
					{
						fscanf(fp, "%*c");//读取换行符 
						string fullPath = string(showFilePath)+string(name+1);
						fullPath = fullPath + line;//得到绝对路径 
						//printf("%s\n", fullPath.c_str());
						struct _stati64 info;
						if (_stati64(fullPath.c_str(), &info) == 0)//获取文件信息 
						{
							struct tm *local;
							local = localtime(&info.st_mtime);//获取文件修改时间 
							int fileTime = local->tm_year<<24 | (local->tm_mon+1)<<18 | local->tm_mday<<12 | local->tm_hour<<6 | local->tm_min;
							//将年月日时分塞到一个int里 
							//bit 0-5 		分
							//bit 6-11 		时
							//bit 12-17 	日
							//bit 19-23 	月
							//bit 24-31 	年 
							FileInfo fileInfo;//填充文件信息结构体 
							fileInfo.fileSize = info.st_size;
							fileInfo.fileTime = fileTime;
							fileInfo.isFolder = (info.st_mode == 16895);
							ve.push_back(fileInfo);
							vec.push_back(string(line));
						}
						else
						{
							printf("cannot get file info of %s, code=%d\n", fullPath.c_str(), GetLastError());
						}
					}
				}
				len = ve.size();//获取文件列表长度
				send(Client, (char*)&len, 4, 0);//发送文件列表长度
				for (int i=0; i<len; i++)
				{
					int len = vec[i].length();//文件名长度 
					send(Client, (char*)&len, 4, 0);//发送文件名长度 
					send(Client, vec[i].c_str(), len, 0);//发送文件名 
					send(Client, (char*)&ve[i], sizeof(FileInfo), 0);//发送文件信息 
				}
				delete[]line;//释放空间 
				delete[]name;
				break;
			}
			
			case DELETED: {//客户端请求删除文件  支持单文件或单文件夹 
				int nameLen;
				recv(Client, (char*)&nameLen, 4, 0);//文件路径长度 
				char* name = new char[nameLen+2];//开辟空间 
				recv(Client, name, nameLen, 0);//接受文件路径 
				name[nameLen]=0;
				string deletePath = string(showFilePath)+string(name+1);//服务器绝对路径 
				printf("get delete path: %s\n", name);
				if (remove(deletePath.c_str()) != 0)//删除单个文件， 删除失败，表示是文件夹 
				{
					string com = "rmdir /S /Q "+deletePath;//cmd命令删除文件夹 
					system(com.c_str());
					printf("delete command: %s\n", com.c_str());
				}
				delete[]name;//释放空间 
				break;
			}
			
			case NEWDIR: {//新建文件夹 
				int nameLen;
				recv(Client, (char*)&nameLen, 4, 0);//文件夹名长度 
				char* name = new char[nameLen+2];//开辟空间 
				recv(Client, name, nameLen, 0);//文件夹名 
				name[nameLen]=0;
				string newPath = string(showFilePath)+string(name+1);//
				if (mkdir(newPath.c_str()))//创建文件夹 
				{
					printf("make dir fail! newPath = %s\n", newPath.c_str());//没什么意义，传输过程没有问题理论上就不会失败 
				}
				delete[]name;//释放空间 
				break;
			}
			
			case RENAME: {//文件重命名 
				int pathLen, newLen, oldLen;
				recv(Client, (char*)&pathLen, 4, 0);//文件路径长度 
				char* path = new char[pathLen+2];
				recv(Client, path, pathLen, 0);//文件路径 
				path[pathLen]=0;
				
				recv(Client, (char*)&oldLen, 4, 0);//旧文件名长度 
				char* oldName = new char[oldLen+2];
				recv(Client, oldName, oldLen, 0);//旧文件名 
				oldName[oldLen]=0;
				
				recv(Client, (char*)&newLen, 4, 0);//新文件名长度 
				char* newName = new char[newLen+2];
				recv(Client, newName, newLen, 0);//新文件名 
				newName[newLen]=0;
				
				string sn = string(showFilePath)+string(path+1)+newName;//新文件名绝对路径 
				string so = string(showFilePath)+string(path+1)+oldName;//旧文件名绝对路径 
				rename(so.c_str(), sn.c_str());//重命名 
				
				delete[]path;//释放空间 
				delete[]oldName;
				delete[]newName;
				break;
			}
			
			case PASTE: {//粘贴复制或剪切的文件   仅支持单个文件或单个文件夹 
				int len_old, len_new, option;
				recv(Client, (char*)&len_old, 4, 0);//源文件所在路径长度 
				char* name_old = new char[len_old+2];
				recv(Client, name_old, len_old, 0);//源文件所在路径 
				name_old[len_old]=0;
				
				recv(Client, (char*)&len_new, 4, 0);//目标路径长度 
				char* name_new = new char[len_new+2];
				recv(Client, name_new, len_new, 0);//目标路径 
				name_new[len_new]=0;
				
				recv(Client, (char*)&option, 4, 0);
				string ss = string(showFilePath)+string(name_new+1);
				string sn = ss + "-pasting";//目标绝对路径 
				string so = string(showFilePath)+string(name_old+1);//源绝对路径 
//				printf("sn=%s, so=%s\n", sn.c_str(), so.c_str());
				delete[]name_old;//释放空间 
				delete[]name_new;
				//使用cmd命令进行复制粘贴 
				if (option&1)//copy
				{
					FILE *fp = fopen(so.c_str(), "r");
					string cmd;
					if (fp)//file
					{
						fclose(fp);
						cmd="copy "+so+" "+sn;
					}
					else//folder
					{
						cmd="xcopy /E /Y /I "+so+" "+sn;
					}
//					thread th(pasteThread, cmd, Client); th.detach();
					system(cmd.c_str());
				}
				else//cut
				{
					string cmd;
					cmd="move /Y "+so+" "+sn;
//					thread th(pasteThread, cmd, Client); th.detach();
					system(cmd.c_str());
				}
				rename(sn.c_str(), ss.c_str());
				int temp=0;
				send(Client, (char*)&temp, 4, 0);
				break;
			}
		}
	}
	printf("thread end\n");
	delete[]buffer;//释放空间 
	closesocket(Client);//关闭套接字 
}

void init()
{
	WSADATA a;
	if (WSAStartup(MAKEWORD(2, 2), &a))//初始化套接字库 
	{
		printf("init fail\n");
		exit(0);
	}
	
	//创建服务器端自己的套接字 
	SOCKET self = socket(AF_INET, SOCK_STREAM, 0);//TCP通信
	//SOCKET self = socket(AF_INET, SOCK_DGRAM, 0);//UDP通信 
	//第一个参数：地址族。 AF 是“Address Family”的简写，INET是“Inetnet”的简写。AF_INET 表示 IPv4 地址。 AF_INET6 表示 IPv6 地址
	//第二个参数：数据传输方式/套接字类型。一般取值 SOCK_STREAM（面向连接） 或 SOCK_DGRAM（无连接） 
	//第三个参数：传输协议，常用的有 IPPROTO_TCP（TCP协议） 和 IPPTOTO_UDP（UDP协议）。可填0，操作系统自动推导类型 
	
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4011);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	
	if ( bind(self, (sockaddr*)&addr, sizeof(sockaddr_in)) )
	{
		printf("bind fail, error code: %d\n", WSAGetLastError());
		exit(0);
	}
	
	if (listen(self, 1) == SOCKET_ERROR)
	{
		printf("can't listen, error code: %d\n", WSAGetLastError());
		exit(0);
	}
	printf("wait connect\n");
	//sockaddr_in addr;
	int len = sizeof(addr);
	while(1)
	{
		Client = accept(self, (sockaddr*)&addr, &len);
//		getThread(Client);break;
		thread th(getThread, Client); th.detach();
	}
	
//	int nNetTimeout = 1;//超时时长
//	setsockopt(Client,SOL_SOCKET,SO_RCVTIMEO,(char *)&nNetTimeout,sizeof(int));

}
int main()
{
	init();
	getchar();
}
