#ifndef CPOP3_H
#define CPOP3_H
#include <string>
#include <memory.h>
#include <iostream>
#include <sstream>
#include <regex.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "base64.h"
#define POP3SERVER_PORT 110
#define MAXDATASIZE 4096

using namespace std;
class CPop3{

	private:
		string emailAccount;
		string password;
		string serverDomainName;
		
		int sockfd;
		string popMessage;
		int buffLength;//接收信息长度
		char buff[MAXDATASIZE];//接收buffer

		void extraInfo();//提取邮件信息
		void printInfo();

	public:
		int emailTotalNum;//邮件总数
	
		string downloadFile;//RETR得到的文件
		string fromWho;//发件人
		string receiveTime;//收件时间
		string subject;//主题
		string message;//邮件内容
		
		string content_encoding;//传输编码方式
		string charset;//使用字符集
		/*
		string to;
		string from;
		string receiveTime;
		*/

		CPop3(string email, string pw, string et);
		~CPop3();
		bool connectToServer();//连接服务器,成功返回 true
		bool loginToServer();//登录,成功返回 true
		bool quitFromServer();//退出，成功返回 true
		void getMaildropStat();//利用STAT获取邮箱的邮件总数
		bool retrieveEmail(int emailNum);//获取邮件内容

		/*
		int getList();
		int getEmail();
		int sendEmail();
		int exitFromServer();
		*/

};
#endif

