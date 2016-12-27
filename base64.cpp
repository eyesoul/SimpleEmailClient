#include "base64.h"

/*Base64编码
 *
 */
char base64Table[64]={
     'A','B','C','D','E','F','G','H',
     'I','J','K','L','M','N','O','P',
     'Q','R','S','T','U','V','W','X',
     'Y','Z','a','b','c','d','e','f',
     'g','h','i','j','k','l','m','n',
     'o','p','q','r','s','t','u','v',
     'w','x','y','z','0','1','2','3',
     '4','5','6','7','8','9','+','/'
};

void base64Encode(string & instr, string & outstr){
	int inlen = instr.length();
	outstr = "";
	unsigned char index;
	//每3个字符一组，编码得到4个字符
	for (int i=0; i<inlen; i+=3){
		index = (instr[i]>>2) & 0x3f;
		outstr.append(1, base64Table[index]);
		if(i+1 == inlen){
			index = (instr[i]<<4) & 0x30;
			outstr.append(1,base64Table[index]);
			outstr.append(2,'=');
			return;
		}
		index = ((instr[i]<<4) & 0x30)|((instr[i+1]>>4) & 0x0f);
		outstr.append(1,base64Table[index]);
		if(i+2 == inlen){
			index = (instr[i+1]<<2) & 0x3c;
			outstr.append(1,base64Table[index]);
			outstr.append(1,'=');
			return;
		}
		index = ((instr[i+1]<<2) & 0x3c)|((instr[i+2]>>6) & 0x03);
		outstr.append(1,base64Table[index]);
		index = instr[i+2] & 0x3f;
		outstr.append(1,base64Table[index]);
	}
}

/*Base64 解码
 *参数为待解码字符串和输出字符串，引用
 *解码成功返回 ture
 */
bool base64Decode(string & instr, string & outstr){
	int inlen = instr.length();
	if((inlen%4)!=0){
		cout<<"Base64 decode error"<<endl;
		return false;
	}
	outstr = "";
	unsigned char index[4];
	unsigned char oc[3];
	char * p;
	//每4个字符做为一组，解码得到3个ascii字符
	for(int i = 0; i<inlen; i+=4){
		
		for(int j=0; j<2; j++){//找前两个字符对应的码
			p = find(base64Table, base64Table+64, instr[i+j]);
			if(p==base64Table+64){
				cout<<"Base64 decode error, no such character in base64Table"<<endl;
				return false;
			}
			else 
				index[j] = (unsigned char)(p-base64Table);
		}
		if(instr[i+2] == '='){//第三个字符若为 = 填充了两字节的 \x00 
			oc[0] = (index[0]<<2)|(index[1]>>4);
			outstr.append(1,oc[0]);
			return true;
		}
		else {
			p = find(base64Table, base64Table+64, instr[i+2]);
			if(p==base64Table+64){
				cout<<"Base64 decode error, no such character in base64Table"<<endl;
				return false;
			}
			else 
				index[2] = (unsigned char)(p-base64Table);
		}

		if(instr[i+3] == '='){//第四个字符若为 = 填充了一个字节的 \x00
			oc[0] = (index[0]<<2)|(index[1]>>4);
			oc[1] = (index[1]<<4)|(index[2]>>2);
			outstr.append(1,oc[0]);
			outstr.append(1,oc[1]);
			return true;
		}
		else{
			p = find(base64Table, base64Table+64, instr[i+3]);
			if(p==base64Table+64){
				cout<<"Base64 decode error, no such character in base64Table"<<endl;
				return false;
			}
			else 
				index[3] = (unsigned char)(p-base64Table);
		}

		oc[0] = (index[0]<<2)|(index[1]>>4);
		oc[1] = (index[1]<<4)|(index[2]>>2);
		oc[2] = (index[2]<<6)|index[3];
		for(int j = 0; j<3; j++)
			outstr.append(1,oc[j]);

	}
	return true;
}
