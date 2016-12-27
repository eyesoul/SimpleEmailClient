#include "stdlib.h"
#include <iostream>
#include <cstring>

#include <QApplication>
#include <QString>
#include <QDialog>

#include "loginDialog.h"
#include "cpop3.h"
class CPop3 * wpop3;

using namespace std;

int main(int argc, char *argv[]){
	QApplication app(argc, argv);
	CLoginDialog * login = new CLoginDialog();
	login->show();
	return app.exec();
}

