#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H
#include "ui_mainWidget.h"
#include <string>
#include <QString>
#include <QApplication>
#include <iostream>
#include "main.h"
using namespace std;
class CMainWidget: public QWidget, public Ui::MainWidget{
	Q_OBJECT
public:
	CMainWidget();

private:
	void reset();

public slots:
	void on_readButton_clicked();
	void on_writeButton_clicked();
};

#endif
