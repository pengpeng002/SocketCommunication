#ifndef __PENGPENG_H__
#define __PENGPENG_H__
#include <bits/stdc++.h>
#include <windows.h>
#include <io.h>
#include "Python.h" 
using namespace std;
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
#define PY(s) PyRun_SimpleString(s);
#define keyPress(nVirtKey)     ((GetKeyState(nVirtKey) & (1<<(sizeof(SHORT)*8-1))) != 0)
#define MAKE(data, type, cmd) ( data = ((type) << 16) | (cmd) )//两个16位组成一个32位整数 
#define MAKEW(data, a, b) (data = (((a)<<8)|(b)))//两个8位组成一个16位整数 
#define HI(data) ((data)>>16)//取32位的高16位 
#define LO(data) ((data)&0xffff)//取32位的低16位 
#define H(data) (((data)>>8)&0xff)//取16位的高8位 
#define L(data) ((data)&0xff)//取16位的低8位 
#define MouseMove(x, y) ::SetCursorPos(x, y) //鼠标移动到对应位置
//#define MouseClick mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0)//模拟鼠标点击 
#define name(p) #p
namespace TestNamespace {
	int main()
	{
		char *line;
		FILE *fp = _popen("dir /s /b *.cpp", "r");
		if (fp)
		{
			while(fscanf(fp, "%[^\n]", line)>0)
			{
				fscanf(fp, "%*c");
				printf("%s\n", line);
			}
		}
	}
	namespace p3 {
		int main() {
			int data, a, b;
			int x=12, y = 13, z=14, u=15;
			MAKEW(a, x, y), MAKEW(b, z, u);
			MAKE(data, a, b);
			cout<<H(HI(data))<<" "<<L(LO(data))<<endl;
		}
	}
}
namespace pengpeng
{	
	namespace __internal
	{
		enum {num_rounds=32};//the round number of encrypt ans decrypt
		void en(unsigned int v[2], unsigned int key[4])//encrypt
		{
			unsigned int v0=v[0],v1=v[1],sum=0,delta=0x9e3779b9;
			for (int i=0; i < num_rounds; i++)
			{
				v0+=(((v1<<4)^(v1>>5))+v1)^(sum+key[sum&3]);
				sum += delta;
				v1+=(((v0<<4)^(v0>>5))+v0)^(sum+key[(sum>>11)&3]);
			}
			v[0]=v0, v[1]=v1;
		}
		
		void de(unsigned int v[2], unsigned int key[4])//decrypt
		{
			unsigned int v0=v[0],v1=v[1],delta=0x9e3779b9,sum=delta*num_rounds;
			for (int i=0; i < num_rounds; i++)
			{
				v1-=(((v0<<4)^(v0>>5))+v0)^(sum+key[(sum>>11)&3]);
				sum -= delta;
				v0-=(((v1<<4)^(v1>>5))+v1)^(sum+key[sum&3]);
			}
			v[0]=v0, v[1]=v1;
		}
	}
	
	static int seed=1;//the seed of rand
	
	void srand(int seed)//set rand seed
	{
		pengpeng::seed=seed;
	}
	
	int random()//get a rand number
	{
		return ((seed = seed * 214013L + 2531011L) >> 16) & 0x7fff;
	}
	
	void Py_Init()
	{
		Py_Initialize();
		if (!Py_IsInitialized()) exit(-1);
	}
	
	void Py_Close()
	{
		Py_Finalize();
	}
	
	string encrypt(const string &path, const string &pass = "#include", const string &out="encrypt")
	{
		FILE *fpin=fopen(path.c_str(),"rb");
		FILE *fpout=fopen(out.c_str(),"wb");
		if (fpin!=NULL && fpout!=NULL)
		{
			while(!feof(fpin))
			{
				int msg[2];
				msg[0]=msg[1]=0;
				if (fread(msg,1,8,fpin)==0)break;
				__internal::en((unsigned int*)msg, (unsigned int*)pass.c_str());
				fwrite(msg,1,8,fpout);
			}
		}
		fclose(fpin);
		fclose(fpout);
	}
	
	string decrypt(const string &path, const string &pass = "#include", const string &out="decrypt")
	{
		FILE *fpin=fopen(path.c_str(),"rb");
		FILE *fpout=fopen(out.c_str(),"wb");
		if (fpin!=NULL && fpout!=NULL)
		{
			while(!feof(fpin))
			{
				int msg[2];
				msg[0]=msg[1]=0;
				if (fread(msg,1,8,fpin)==0)break;
				__internal::de((unsigned int*)msg, (unsigned int*)pass.c_str());
				fwrite(msg,1,8,fpout);
			}
		}
		fclose(fpin);
		fclose(fpout);
	}
	
	void MouseClick(const int x,const int y)//mouse left click x, y
	{
		POINT point;
		GetCursorPos(&point);
		DWORD dx = x * 65536 / GetSystemMetrics(SM_CXSCREEN);//计算要点击的位置，将像素坐标转换鼠标能认识的坐标
		DWORD dy = y * 65536 / GetSystemMetrics(SM_CYSCREEN);
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP,dx, dy, 0, GetMessageExtraInfo());//移动鼠标，模拟点击鼠标左键
		SetCursorPos(point.x, point.y);//恢复鼠标到原来的位置
	}
	
	bool isUtf8(const char *str)//check if utf-8
	{
		unsigned int nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节
		unsigned char chr = *str;
		bool bAllAscii = true;
		for (unsigned int i = 0; str[i] != '\0'; ++i)
		{
			chr = *(str + i);//判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
			if (nBytes == 0 && (chr & 0x80) != 0) bAllAscii = false;
			if (nBytes == 0 && chr >= 0x80)//如果不是ASCII码,应该是多字节符,计算字节数
			{
				if (chr >= 0xFC && chr <= 0xFD) nBytes = 6;
				else if (chr >= 0xF8) nBytes = 5;
				else if (chr >= 0xF0) nBytes = 4;
				else if (chr >= 0xE0) nBytes = 3;
				else if (chr >= 0xC0) nBytes = 2;
				else return false;
				nBytes--;
			}
			else if (nBytes != 0)
			{
				if ((chr & 0xC0) != 0x80) return false;//多字节符的非首字节,应为 10xxxxxx
				nBytes--;//减到为零为止
			}
		}
		if (nBytes != 0) return false;
		return true;
	}
	
	string u2g(const char *src_str)//utf-8 to gbk
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
		wchar_t* wszGBK = new wchar_t[len + 1];
		memset(wszGBK, 0, len * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
		len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
		char* szGBK = new char[len + 1];
		memset(szGBK, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
		string strTemp(szGBK);
		if (wszGBK) delete[] wszGBK;
		if (szGBK) delete[] szGBK;
		return strTemp;
	}
	
	string g2u(const char *src_str)//gbk to utf-8
	{
		int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
		len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[len + 1];
		memset(str, 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
		string strTemp = str;
		if (wstr) delete[] wstr;
		if (str) delete[] str;
		return strTemp;
	}
	
	void getFiles( string path, vector<string>& files )
	{
		int hFile = 0;
		struct _finddata_t fileinfo;
		string p;
		if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)
		{
			do
			{
				if((fileinfo.attrib &  _A_SUBDIR))
				{
					if(strcmp(fileinfo.name,".") && strcmp(fileinfo.name,"..") )
						getFiles( p.assign(path).append("\\").append(fileinfo.name), files );
				}
				else
				{
					string s=fileinfo.name;
					if (s.substr(s.length()-4) == ".txt")
					files.push_back(p.assign(path).append("\\").append(fileinfo.name) );
				}
			}
			while(_findnext(hFile, &fileinfo)  == 0);
			_findclose(hFile);
		}
	}

	string split(const string &str, vector<string> *ve=NULL, char split=' ')//split str by split to ve
	{
		istringstream iss(str);
		string ret;
		while(getline(iss, ret, split)) if (ve) ve->push_back(ret);
		return ret;
	}
	
	int print(const char* fmt, ...)//my printf
	{
	    char buf[256];
	    va_list arg = (va_list)((char*)(&fmt) + 4); 
	    int ret = vsprintf(buf, fmt, arg); 
	    printf(buf);
	    return ret; 
	}
	
	inline int fgetline(FILE* f, char* a)
	{
		int x=0;
		while (~fscanf(f, "%c", &a[x]))
		{
			if (a[x]=='\n' || a[x]=='\r' || a[x]=='\0')
			{
				a[x]='\0';
				break;
			}
			x++;
		}
		return feof(f) ? -1 : x;
//		int x=fscanf(f, "%[^\r\n]", a);
//		fscanf(f, "%*c");
//		return x;
	}
	
	inline int getline(char* a)//get a line include enter 
	{
		return fgetline(stdin, a); 
		int x=scanf("%[^\n]", a);
		scanf("%*c");
		return x;
	}
	
	inline string getCompileDate()
	{
		return __DATE__;
	}
	
	inline string getCompileTime()
	{
		return __TIME__;
	}
	
	void notMultiRun()
	{
		if(CreateMutex(0, 0, "鹏鹏") != 0)
		{
			if (GetLastError() == ERROR_ALREADY_EXISTS)
			{
				MessageBox(0, "already exists\n", "note", 0);
			}
		}
	}
}//end namespace
#endif
