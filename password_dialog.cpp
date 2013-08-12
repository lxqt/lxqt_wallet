#include "password_dialog.h"
#include "ui_password_dialog.h"

password_dialog::password_dialog( QWidget * parent ) : QDialog( parent ),m_ui(new Ui::password_dialog )
{
	m_ui->setupUi( this ) ;
	connect( m_ui->pushButtonSend,SIGNAL( clicked() ),this,SLOT( pbSend() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
}

void password_dialog::ShowUI( const QString& walletName,const QString& applicationName )
{
	m_ui->labelHeader->setText( tr( "application %1 wants to open %2 wallet" ).arg( applicationName ).arg( walletName ) ) ;
	this->show() ;
}

password_dialog::~password_dialog()
{
	delete m_ui;
}

void password_dialog::pbSend()
{
	emit password( m_ui->lineEditKey->text() ) ;
}

void password_dialog::pbCancel()
{
	emit cancelled() ;
	this->HideUI() ;
}

void password_dialog::passwordIsCorrect( bool correctPassword )
{
	if( correctPassword ){
		this->hide() ;
	}else{
		QMessageBox msg( this ) ;
		msg.setText( tr( "wallet could not be opened with the presented key" ) ) ;
		msg.exec() ;
	}
}

void password_dialog::HideUI()
{
	this->deleteLater() ;
}

void password_dialog::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}
