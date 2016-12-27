#include "cpop3.h"

class CPop3;
CPop3::CPop3(string email, string pw, string et):
	emailAccount(email+et),password(pw)
{
	if(et == "@163.com")
		serverDomainName = "pop.163.com";
	else if(et == "@pku.edu.cn")
		serverDomainName = "mail.pku.edu.cn";
	else
		serverDomainName = "";
	popMessage = "";
	emailTotalNum = 0;
	downloadFile = "";
	
	fromWho = "";
	receiveTime = "";
	subject = "";
	message = "";
	
}

CPop3::~CPop3(){
	close(sockfd);//关闭socket，头文件 <unistd.h>
}

/*通过socket与服务器进行连接，连接成功后服务器会返回信息 +OK ...
 *服务器返回的信息以 \n\r 结尾
 *连接成功后，进入状态 AUTHORIZATION，等待登录
 */
bool CPop3::connectToServer(){
	struct hostent *host;
	struct sockaddr_in serv_addr;
	string serv_name="pop";
	//获取服务器地址
	if( ( host=gethostbyname(serverDomainName.data()))==NULL ){
		perror("gethostbyname error");
		return false;
	}
	//创建socket
	if( ( sockfd=socket(AF_INET, SOCK_STREAM, 0))==-1 ){
		perror("socket error");
		return false;
	}
	//填充服务器地址端口信息
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(POP3SERVER_PORT);
	serv_addr.sin_addr = *((struct in_addr *)host->h_addr);
	//发起连接	
	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1 ){
		perror("connect error");
		return false;
	}
	//接收打印返回信息
	buffLength = recv(sockfd, buff, sizeof(buff), 0);
	buff[buffLength-1]='\0';
	cout<<"S: "<<buff<<endl;

	return true;
}

/*输入用户名和密码，登录服务器
 *指令："USER somebody@163.com\n\r" 成功返回 +OK ...，出错返回 -ERR ...
 *		"PASS ****************\n\r"
 *登录成功后进入 TRANSACTION 状态
 */
bool CPop3::loginToServer(){
	popMessage = "USER "+emailAccount+"\n" ;
	send(sockfd, popMessage.data(), popMessage.size(), 0);
	cout<<"Sent USER ..."<<endl;
	buffLength = recv(sockfd, buff, sizeof(buff), 0);
	buff[buffLength-1]='\0';
	cout<<"S: "<<buff<<endl;
	
	popMessage = "PASS "+password+"\n";
	send(sockfd, popMessage.data(), popMessage.size(), 0);
	cout<<"Sent PASS ..."<<endl;
	buffLength = recv(sockfd, buff, sizeof(buff), 0);
	buff[buffLength-1]='\0';
	cout<<"S: "<<buff<<endl;
/*
	popMessage = "TOP 1 4\n";
	send(sockfd, popMessage.data(), popMessage.size(), 0);
	cout<<"Sent TOP ..."<<endl;
	int n = 4;
	while(--n){
	buffLength = recv(sockfd, buff, sizeof(buff), 0);
	buff[buffLength-1]='\0';
	cout<<"S: "<<buff<<endl;
	}
*/
	getMaildropStat();
	//retrieveEmail(emailTotalNum);
	//quitFromServer();
	return true;
}

/*从服务器退出
 *TRANSACTION 状态操作结束后，QUIT 退出进入UPDATE状态，保存有关删除的操作 
 *成功返回 +OK ... 失败返回 -ERR ...
 */
bool CPop3::quitFromServer(){
	popMessage = "QUIT\n";
	send(sockfd, popMessage.data(), popMessage.size(), 0);
	cout<<"Sent QUIT"<<endl;
	buffLength = recv(sockfd, buff, sizeof(buff), 0);
	buff[buffLength-1]='\0';
	cout<<"S: "<<buff<<endl;
	return true;
}

/*获取邮箱统计信息，利用 STAT
 *返回信息为 "+OK <邮件总数> <所占大小>\n\r" 
 */
void CPop3::getMaildropStat(){
	popMessage = "STAT\n";
	send(sockfd, popMessage.data(), popMessage.size(), 0);
	cout<<"Sent STAT"<<endl;
	buffLength = recv(sockfd, buff, sizeof(buff), 0);
	buff[buffLength-1]='\0';
	cout<<"S: "<<buff<<endl;
	//提取邮件总数信息
	if(buff[0]=='+'){
		emailTotalNum = 0;
		int i=4;
		while( buff[i]!=' ' ){
			emailTotalNum = 10*emailTotalNum + (buff[i]-'0');
			i++;
		}
	}
}

/*获取邮件内容，利用 "RETR <邮件序号>\n" 
 *成功返回 "+OK <size> ...\n\r<邮件内容>\n\r.\n\r"
 *失败返回 "-ERR ..."
 */
bool CPop3::retrieveEmail(int emailNum){
	string endMark;
	//邮件序号 int 转 string
	string emailNumStr;
	stringstream ss;
	ss<<emailNum;
	ss>>emailNumStr;
	//构造指令并发送
	popMessage = "RETR "+emailNumStr+"\n";
	send(sockfd, popMessage.data(), popMessage.size(), 0);
	cout<<"Sent RETR ..."<<endl;
	//接收返回的信息
	buffLength = recv(sockfd, buff, sizeof(buff), 0);
	buff[buffLength-1]='\0';
	cout<<"S: "<<buff<<endl;
	if(buff[0]=='+'){//接收邮件内容
		downloadFile="";
		do{
			endMark = "";
			buffLength = recv(sockfd, buff, sizeof(buff), 0);
			downloadFile.append(buff);
			for(int i=5; i>0; i--)
				endMark.append(1,buff[buffLength-i]);
			//cout<<(int)endMark[0]<<(int)endMark[1]<<endl;
		}while(endMark != "\r\n.\r\n");
		extraInfo();
		printInfo();
	}
	else 
		return false;
	return true;
}

/*提取下载邮件中的信息，并对邮件具体内容进行Base64解码
 *
 */
void CPop3::extraInfo(){
	const char * df = downloadFile.c_str();
	//提取Date
	cout<< df;
	char pattern[100]="Date: (.+?) \\+.+?\r\n";
	const size_t nmatch = 5;
	int s;
	regmatch_t pmatch[5];
	regex_t reg;
	regcomp(&reg, pattern, REG_EXTENDED);
	if( (s=regexec(&reg, df , nmatch, pmatch, 0))==REG_NOMATCH){
		cout<<"No match, date"<<endl;
	}
	else{
		cout<<"Matched, date"<<endl;
		receiveTime = string(df, pmatch[1].rm_so, pmatch[1].rm_eo-pmatch[1].rm_so);
		//cout<<receiveTime;
	}
	regfree(&reg);

	//提取From
	memset(pattern, '\0', sizeof(pattern));
	strcpy(pattern, "From: .*? <(.+?)>\r\nTo");
	regcomp(&reg, pattern, REG_EXTENDED);
	if( (s=regexec(&reg, df, nmatch, pmatch, 0))==REG_NOMATCH){
		cout<<"No match, from"<<endl;
	}
	else{
		cout<<"Matched, from"<<endl;
		fromWho = string(df, pmatch[1].rm_so, pmatch[1].rm_eo-pmatch[1].rm_so);
		//cout<<fromWho;
	}
	regfree(&reg);

	//提取Subject
	memset(pattern, '\0', sizeof(pattern));
	strcpy(pattern, "Subject: (.+?)\r\nFrom");
	regcomp(&reg, pattern, REG_EXTENDED);
	if( (s=regexec(&reg, df, nmatch, pmatch, 0))==REG_NOMATCH){
		cout<<"No match, subject"<<endl;
	}
	else{
		cout<<"Matched, subject"<<endl;
		subject = string(df, pmatch[1].rm_so, pmatch[1].rm_eo-pmatch[1].rm_so);
		//cout<<subject;
	}
	regfree(&reg);

	//提取字符集类型
	memset(pattern, '\0', sizeof(pattern));
	strcpy(pattern, "Content-Type: text/plain; charset=(.+?)\r\nContent-Transfer-Encoding");
	regcomp(&reg, pattern, REG_EXTENDED);
	if( (s=regexec(&reg, df, nmatch, pmatch, 0))==REG_NOMATCH){
		cout<<"No match, charset"<<endl;
	}
	else{
		cout<<"Matched, charset"<<endl;
		charset = string(df, pmatch[1].rm_so, pmatch[1].rm_eo-pmatch[1].rm_so);
	}
	regfree(&reg);

	//提取编码方式 Base64 | 7bit
	memset(pattern, '\0', sizeof(pattern));
	strcpy(pattern, "Content-Transfer-Encoding: (.{4,6})\r\n");
	regcomp(&reg, pattern, REG_EXTENDED);
	if( (s=regexec(&reg, df, nmatch, pmatch, 0))==REG_NOMATCH){
		cout<<"No match, content encoding"<<endl;
	}
	else{
		cout<<"Matched, content encoding"<<endl;
		content_encoding = string(df, pmatch[1].rm_so, pmatch[1].rm_eo-pmatch[1].rm_so);
	}
	regfree(&reg);

	//提取邮件主体
	memset(pattern, '\0', sizeof(pattern));
	string undecoded_content;
	strcpy(pattern,"\r\n\r\n(.+?)\r\n\\.\r\n");
	regcomp(&reg, pattern, REG_EXTENDED);
	if( (s=regexec(&reg, df, nmatch, pmatch, 0))==REG_NOMATCH){
		cout<<"No match, content"<<endl;
	}
	else{
		cout<<"Matched, content"<<endl;
		undecoded_content = string(df, pmatch[1].rm_so, pmatch[1].rm_eo-pmatch[1].rm_so);
	}
	regfree(&reg);
	
	//解码邮件内容
	if(content_encoding == "7bit"){
		message = undecoded_content;
	}
	else if(content_encoding == "base64"){
		cout<<"Base64 decoding ..."<<endl;
		string str = "";//去\n\r的Base64编码串
		for(unsigned int i=0; i<undecoded_content.length(); i++){
			if((undecoded_content[i] != '\n') && (undecoded_content[i] != '\r')){
				str.append(1,undecoded_content[i]);
			}
		}
		cout<<str<<endl;
		if(base64Decode(str, message)){
			cout<<"base64Decode successful!"<<endl;
		}
		else{
			cout<<"base64Decode failed!"<<endl;
			message = "";
		}
	}
}

/*
 *
 */
void CPop3::printInfo(){
	cout<<"Content transfer encoding: "<<content_encoding<<endl;
	cout<<"Charset: "<<charset<<endl;
	cout<<"From: "<<fromWho <<endl;
	cout<<"Receive time: "<<receiveTime<<endl;
	cout<<"Subject: "<<subject<<endl;
	cout<<"Content:"<<endl;
	cout<<message<<endl;
}
