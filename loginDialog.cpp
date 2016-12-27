#include "loginDialog.h"
#include <QApplication>
#include <string> 
#include <QString>
#include <iostream>
#include "main.h"
#include "mainWidget.h"
using namespace std;
class CLoginDialog;
CLoginDialog::CLoginDialog(){
	setupUi(this);
	connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
	//设置账户输入字符为字母数字._等字符，6-18个
	QRegExp rx_a("[a-zA-Z0-9][a-zA-Z0-9._]{5,17}$");
	QValidator *vali_a = new QRegExpValidator(rx_a, this);
	accountLineEdit->setValidator(vali_a);
	//设置密码字符非空，6-20个
	QRegExp rx_p("^[\\S]{6,20}$");
	QValidator *vali_p = new QRegExpValidator(rx_p, this);
	passwdLineEdit->setValidator(vali_p);
	//设置QComboBox
	et = "@pku.edu.cn";
	emailTypeComboBox->insertItem(0,"@pku.edu.cn");
	emailTypeComboBox->insertItem(1,"@163.com");
	connect(emailTypeComboBox, SIGNAL(activated(const QString &)), this, SLOT(setEmailType(QString)));
}

void CLoginDialog::setEmailType(QString str){
	et = str.toStdString();
	//cout<<et<<endl;
}

void CLoginDialog::reset(){
	cout<<"Reset account & password"<<endl;
	et = "@pku.edu.cn";
	emailTypeComboBox->setCurrentIndex(0);
	accountLineEdit->setText("");
	passwdLineEdit->setText("");
}

string CLoginDialog::getAccount(){
	string account;
	account = accountLineEdit->text().toStdString();
	return account;
}

string CLoginDialog::getPassword(){
	string pwd;
	pwd  = passwdLineEdit->text().toStdString();
	return pwd;
}

string CLoginDialog::getEmailType(){
	return et;
}

void CLoginDialog::on_loginButton_clicked(){
	if( !wpop3 ){//实例化wpop3
		wpop3 = new CPop3(getAccount(),getPassword(),getEmailType());
	}
	else{
		cout<<"wpop3 has been exist"<<endl;
		return;
	}
	if( !wpop3->connectToServer() ){//连接邮件服务器
		delete wpop3;
		wpop3 = NULL;
		cout<<"Connect Error! Can't connect to server. Please check your network."<<endl;
		reset();
		return;
	}
	if( wpop3->loginToServer() ){//登录邮件服务器，成功
		cout<<"Login successful."<<endl;	
		CMainWidget * mainW = new CMainWidget();
		mainW->show();
		this->close();
	}
	else{//失败
		delete wpop3;
		wpop3 = NULL;
		cout<<"Login Error! Can't login to the server. Please check your account & password"<<endl;
		reset();
		return;
	}
	

}
