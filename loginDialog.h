#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H
class QDialog;
#include "ui_loginDialog.h"
#include <string>
using namespace std;
class CLoginDialog: public QDialog, public Ui::LoginDialog{
	Q_OBJECT
public:
	CLoginDialog();
	string getAccount();
	string getPassword();
	string getEmailType();
private:
	string et;
	void reset();
public slots:
	void setEmailType(QString str);	
	void on_loginButton_clicked();
};

#endif
