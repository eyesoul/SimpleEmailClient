#include "mainWidget.h"

using namespace std;
class CMainWidget;

CMainWidget::CMainWidget(){
	setupUi(this);
	connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
}

void CMainWidget::on_readButton_clicked(){
	reset();
	wpop3->retrieveEmail(wpop3->emailTotalNum);
	fromLabel->setText(wpop3->fromWho.c_str());
	timeLabel->setText(wpop3->receiveTime.c_str());
	subjectLabel->setText(wpop3->subject.c_str());
	emailBrowser->setText(wpop3->message.c_str());
}

void CMainWidget::on_writeButton_clicked(){
	cout<<"Write now"<<endl;
}

void CMainWidget::reset(){
	fromLabel->setText("");
	timeLabel->setText("");
	subjectLabel->setText("");
	emailBrowser->setText("");
	wpop3->getMaildropStat();	
}
