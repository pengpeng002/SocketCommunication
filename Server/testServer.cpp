#include <bits/stdc++.h>
#include "winsock2.h"
#include <windows.h>
#include "pengpeng.h"
using namespace std;
const char* showFilePath = "E:\\testDisk";//չʾ���û����ļ��Ĵ洢·�������ͻ�����ʾĿ¼�ĸ�·��������Ӧ�ø���Ȩ�޲�ͬ��·����ͬ������ʱ������Ȩ�� 
SOCKET Client;
//const int bufferSize = 1024000;
//char buffer[bufferSize];
enum {//�������ɽ��ܵ�����ָ�����ָ�ͬ��ִ�еĶ�����ͬ 
	EXIT, LOGIN, UPLOAD, DOWNLOAD, REFRESH, DELETED, NEWDIR, RENAME, PASTE
};
bool checkPassword(char* name, char* pass)//����û��������Ƿ���ȷ 
{
	return strcmp(name, "admin")==0 && strcmp(pass, "admin")==0;
}
struct FileInfo {//�����ͻ��˵��ļ��б������ 
	long long fileSize;//�ļ���С 
	int fileTime;//�ļ��޸�ʱ�� 
	bool isFolder;//�Ƿ����ļ��� 
};
int getPathName(const char* path)//����·����ȡ�ļ��� �������һ��\\ ���ַ����е�λ�� 
{
	int len = strlen(path);
	int st = 0, ed = 0;
	for (int i = len - 1; i >= 0; i--)
	{
		if (ed == 0 && path[i] == '.')
		{
			ed = i;//û������ 
		}
		else if (st == 0 && path[i] == '\\')
		{
			st = i;
			break;
		}
	}
	return st;
}

void getThread(SOCKET Client)//���ܿͻ���ָ����߳� 
{
	const int bufferSize = 1024*1024*20;
	char* buffer = new char[bufferSize]; //�ļ�����Ļ����� 
	while(1)
	{
		int opt;
		printf("wait recv\n");
		int len = recv(Client, (char*)&opt, 4, 0);//���ܵ���ָ�� 
		printf("len = %d, get opt = %d\n", len, opt);
		if (len <= 0) break;//�ͻ��˷������˳�������ܲ����Ͽ�����ָ�� 
		
		switch (opt)
		{
			case EXIT: {//�ͻ���ϣ����������Ͽ�����  �ͻ��������˳� 
				closesocket(Client);//�ر��׽��� 
				delete[]buffer;//�ͷŻ������������ڴ�й© 
				return ;
				break;
			}
			case LOGIN: {//�ͻ��˵�¼ 
				int nameLen, passLen;
				recv(Client, (char*)&nameLen, 4, 0);//�û������� 
				recv(Client, (char*)&passLen, 4, 0);//���볤�� 
				char *name = new char[nameLen+2];//�����û������� 
				char* pass = new char[passLen+2];//������������ 
				recv(Client, name, nameLen, 0);//�����û��� 
				recv(Client, pass, passLen, 0);//�������� 
				name[nameLen]=0, pass[passLen]=0;//�ֶ����ַ���β�����\0 
				bool ans = checkPassword(name, pass);//����Ƿ�Ϸ��û� 
				send(Client, (char*)&ans, 1, 0);//���߿ͻ��˼���� 
				delete[]name;//�ͷ�����Ŀռ䣬�����ڴ�й¶ 
				delete[]pass;
				break;
			}
			
			case UPLOAD: {//�ͻ��������ϴ��ļ�   �����ļ��ϴ� 
				int nameLen;
				long long fileLen;
				recv(Client, (char*)&nameLen, 4, 0);//�ļ������ļ�·�� ���� 
				char *path = new char[nameLen+5];//�����ļ������� 
				recv(Client, path, nameLen, 0);//�����ļ����� ע���ͻ��˷�������·��Ϊ�ͻ�������Ϊ�ľ���·����һ����\\\\��ͷ 
				path[nameLen]=0;
				recv(Client, (char*)&fileLen, 8, 0);//�ļ����� 
				if (fileLen == 0)
				{
					delete[]path;
					break;
				}
				string savePath = string(showFilePath)+string(path+1);	//���浽��������·�������ｫ�ͻ��˵ľ���·��ת�����������ķ������ϵľ���·�� 
				FILE *fp = fopen(savePath.c_str(), "wb");//��д����ʽ���ļ� 
//				if (!fp) printf("error!!\n");//û������ 
				long long saveLen = 0;//�ܹ����յ������ݳ��� 
				while (saveLen < fileLen)
				{
					int readLen = recv(Client, buffer, min((long long)bufferSize, fileLen-saveLen), 0);//�������� 
					fwrite(buffer, readLen, 1, fp);//�����յ�������д���ļ� 
					saveLen += readLen;//�����ܽ��ճ��� 
					printf("\r%02.2lf %lld / %lld", saveLen * 100.0f / fileLen, saveLen, fileLen);//��ӡ�ļ����ս��� 
				}
				printf("\nsave end\n");
				fclose(fp);//�ر��ļ����������������ļ�д��Ӳ�� 
				delete[]path;//�ͷſؼ��������ڴ�й¶ 
				break;
			}
			
			case DOWNLOAD: {//�ͻ������������ļ�   �����ļ����� 
				int nameLen;
				recv(Client, (char*)&nameLen, 4, 0);//�ļ������ļ�·�� ���� 
				char *path = new char[nameLen+5];//�����ļ������� 
				recv(Client, path, nameLen, 0);//�����ļ����� ע���ͻ��˷�������·��Ϊ�ͻ�������Ϊ�ľ���·����һ����\\\\��ͷ 
				path[nameLen]=0;
				string str = string(showFilePath)+string(path+1);//�ڷ������ϱ����·��   ���ｫ�ͻ��˵ľ���·��ת�����������ķ������ϵľ���·��
				struct _stati64 info;
				_stati64(str.c_str(), &info);//��ȡ�ļ���Ϣ 
				long long len = info.st_size;
				FILE* fp = fopen(str.c_str(), "rb");//���ļ� 
				send(Client, (char*)&len, 8, 0);//�����ļ����� 
				if (len > INT_MAX) len = 0;
				fseek(fp, 0, SEEK_SET);//�ļ�ָ���Ƶ��ļ���ͷ
				long long sendLen = 0;//�ѷ��͵����ݳ��� 
				while (sendLen < len && !feof(fp))
				{
					int readLen = fread(buffer, 1, bufferSize, fp);//��ȡ�ļ����� 
					send(Client, buffer, readLen, 0);//�������� 
					sendLen += readLen;//�����ѷ��ͳ��� 
					printf("\r%02.2lf %lld / %lld", sendLen * 100.0f / len, sendLen, len);//��ӡ�ļ����ͽ��� 
				}
				fclose(fp);//�ر��ļ�ָ�� 
				delete[]path;//�ͷſؼ��������ڴ�й¶ 
				printf("\nsend end\n");
				break;
			}
			
			case REFRESH: {//ˢ����ʾ 
				int len;
				recv(Client, (char*)&len, 4, 0);//��ˢ����ʾ��·���ĳ��� 
				char *name = new char[len+2];
				recv(Client, name, len, 0);//ˢ����ʾ��·�� 
				name[len]=0;				
				string com = "dir "+string(showFilePath)+string(name+1)+" /b";//�ùܵ���ȡ��ˢ��·���µ������ļ�   �����������ļ���ϵͳ�ļ��� 
				printf("command : %s\n", com.c_str());
				FILE* fp = _popen(com.c_str(), "r");//�Զ�����ʽ�򿪹ܵ�����ȡdir����ִ�н�� 
				char *line = new char[MAX_PATH+5];//һ�еĳ��ȣ����ļ�����󳤶� 
				vector<FileInfo> ve;//�ļ���Ϣ���� 
				vector<string> vec;//�ļ������飬�ʼ���ʱ�����ļ�����ֻ���������� 
				if (fp)
				{
					while(fscanf(fp, "%[^\n]", line)>0)//��ȡһ�� 
					{
						fscanf(fp, "%*c");//��ȡ���з� 
						string fullPath = string(showFilePath)+string(name+1);
						fullPath = fullPath + line;//�õ�����·�� 
						//printf("%s\n", fullPath.c_str());
						struct _stati64 info;
						if (_stati64(fullPath.c_str(), &info) == 0)//��ȡ�ļ���Ϣ 
						{
							struct tm *local;
							local = localtime(&info.st_mtime);//��ȡ�ļ��޸�ʱ�� 
							int fileTime = local->tm_year<<24 | (local->tm_mon+1)<<18 | local->tm_mday<<12 | local->tm_hour<<6 | local->tm_min;
							//��������ʱ������һ��int�� 
							//bit 0-5 		��
							//bit 6-11 		ʱ
							//bit 12-17 	��
							//bit 19-23 	��
							//bit 24-31 	�� 
							FileInfo fileInfo;//����ļ���Ϣ�ṹ�� 
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
				len = ve.size();//��ȡ�ļ��б���
				send(Client, (char*)&len, 4, 0);//�����ļ��б���
				for (int i=0; i<len; i++)
				{
					int len = vec[i].length();//�ļ������� 
					send(Client, (char*)&len, 4, 0);//�����ļ������� 
					send(Client, vec[i].c_str(), len, 0);//�����ļ��� 
					send(Client, (char*)&ve[i], sizeof(FileInfo), 0);//�����ļ���Ϣ 
				}
				delete[]line;//�ͷſռ� 
				delete[]name;
				break;
			}
			
			case DELETED: {//�ͻ�������ɾ���ļ�  ֧�ֵ��ļ����ļ��� 
				int nameLen;
				recv(Client, (char*)&nameLen, 4, 0);//�ļ�·������ 
				char* name = new char[nameLen+2];//���ٿռ� 
				recv(Client, name, nameLen, 0);//�����ļ�·�� 
				name[nameLen]=0;
				string deletePath = string(showFilePath)+string(name+1);//����������·�� 
				printf("get delete path: %s\n", name);
				if (remove(deletePath.c_str()) != 0)//ɾ�������ļ��� ɾ��ʧ�ܣ���ʾ���ļ��� 
				{
					string com = "rmdir /S /Q "+deletePath;//cmd����ɾ���ļ��� 
					system(com.c_str());
					printf("delete command: %s\n", com.c_str());
				}
				delete[]name;//�ͷſռ� 
				break;
			}
			
			case NEWDIR: {//�½��ļ��� 
				int nameLen;
				recv(Client, (char*)&nameLen, 4, 0);//�ļ��������� 
				char* name = new char[nameLen+2];//���ٿռ� 
				recv(Client, name, nameLen, 0);//�ļ����� 
				name[nameLen]=0;
				string newPath = string(showFilePath)+string(name+1);//
				if (mkdir(newPath.c_str()))//�����ļ��� 
				{
					printf("make dir fail! newPath = %s\n", newPath.c_str());//ûʲô���壬�������û�����������ϾͲ���ʧ�� 
				}
				delete[]name;//�ͷſռ� 
				break;
			}
			
			case RENAME: {//�ļ������� 
				int pathLen, newLen, oldLen;
				recv(Client, (char*)&pathLen, 4, 0);//�ļ�·������ 
				char* path = new char[pathLen+2];
				recv(Client, path, pathLen, 0);//�ļ�·�� 
				path[pathLen]=0;
				
				recv(Client, (char*)&oldLen, 4, 0);//���ļ������� 
				char* oldName = new char[oldLen+2];
				recv(Client, oldName, oldLen, 0);//���ļ��� 
				oldName[oldLen]=0;
				
				recv(Client, (char*)&newLen, 4, 0);//���ļ������� 
				char* newName = new char[newLen+2];
				recv(Client, newName, newLen, 0);//���ļ��� 
				newName[newLen]=0;
				
				string sn = string(showFilePath)+string(path+1)+newName;//���ļ�������·�� 
				string so = string(showFilePath)+string(path+1)+oldName;//���ļ�������·�� 
				rename(so.c_str(), sn.c_str());//������ 
				
				delete[]path;//�ͷſռ� 
				delete[]oldName;
				delete[]newName;
				break;
			}
			
			case PASTE: {//ճ�����ƻ���е��ļ�   ��֧�ֵ����ļ��򵥸��ļ��� 
				int len_old, len_new, option;
				recv(Client, (char*)&len_old, 4, 0);//Դ�ļ�����·������ 
				char* name_old = new char[len_old+2];
				recv(Client, name_old, len_old, 0);//Դ�ļ�����·�� 
				name_old[len_old]=0;
				
				recv(Client, (char*)&len_new, 4, 0);//Ŀ��·������ 
				char* name_new = new char[len_new+2];
				recv(Client, name_new, len_new, 0);//Ŀ��·�� 
				name_new[len_new]=0;
				
				recv(Client, (char*)&option, 4, 0);
				string ss = string(showFilePath)+string(name_new+1);
				string sn = ss + "-pasting";//Ŀ�����·�� 
				string so = string(showFilePath)+string(name_old+1);//Դ����·�� 
//				printf("sn=%s, so=%s\n", sn.c_str(), so.c_str());
				delete[]name_old;//�ͷſռ� 
				delete[]name_new;
				//ʹ��cmd������и���ճ�� 
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
	delete[]buffer;//�ͷſռ� 
	closesocket(Client);//�ر��׽��� 
}

void init()
{
	WSADATA a;
	if (WSAStartup(MAKEWORD(2, 2), &a))//��ʼ���׽��ֿ� 
	{
		printf("init fail\n");
		exit(0);
	}
	
	//�������������Լ����׽��� 
	SOCKET self = socket(AF_INET, SOCK_STREAM, 0);//TCPͨ��
	//SOCKET self = socket(AF_INET, SOCK_DGRAM, 0);//UDPͨ�� 
	//��һ����������ַ�塣 AF �ǡ�Address Family���ļ�д��INET�ǡ�Inetnet���ļ�д��AF_INET ��ʾ IPv4 ��ַ�� AF_INET6 ��ʾ IPv6 ��ַ
	//�ڶ������������ݴ��䷽ʽ/�׽������͡�һ��ȡֵ SOCK_STREAM���������ӣ� �� SOCK_DGRAM�������ӣ� 
	//����������������Э�飬���õ��� IPPROTO_TCP��TCPЭ�飩 �� IPPTOTO_UDP��UDPЭ�飩������0������ϵͳ�Զ��Ƶ����� 
	
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
	
//	int nNetTimeout = 1;//��ʱʱ��
//	setsockopt(Client,SOL_SOCKET,SO_RCVTIMEO,(char *)&nNetTimeout,sizeof(int));

}
int main()
{
	init();
	getchar();
}
