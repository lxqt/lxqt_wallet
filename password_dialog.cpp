#include "password_dialog.h"
#include "ui_password_dialog.h"

password_dialog::password_dialog( QWidget * parent ) : QDialog( parent ),m_ui(new Ui::password_dialog )
{
	m_ui->setupUi( this ) ;
	this->setFixedSize( this->size() ) ;
	connect( m_ui->pushButtonSend,SIGNAL( clicked() ),this,SLOT( pbSend() ) ) ;
	connect( m_ui->pushButtonCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;

	m_ui->textEdit_2->setVisible( false ) ;
	m_ui->textEdit->setVisible( false ) ;
	m_ui->pushButtonOK->setVisible( false ) ;

	m_closeUIOnKeySend = false ;
}

void password_dialog::ShowUI( const QString& walletName,const QString& applicationName )
{
	m_createWallet = false ;
	QString msg = m_ui->textEdit->toHtml() ;
	m_ui->labelWalletDoesNotExist->setVisible( false ) ;
	m_ui->labelHeader->setText( msg.arg( applicationName ).arg( walletName ) ) ;
	this->show() ;
}

void password_dialog::ShowUI( const QString& walletName )
{
	m_createWallet = true ;
	m_ui->textEdit->setVisible( false ) ;
	m_ui->labelWalletDoesNotExist->setVisible( true ) ;
	m_ui->labelWalletDoesNotExist->setText( m_ui->textEdit_2->toHtml().arg( walletName ) ) ;
	m_ui->labelHeader->setVisible( false ) ;
	m_ui->lineEditKey->setVisible( false ) ;
	m_ui->pushButtonSend->setText( tr( "yes" ) ) ;
	m_ui->pushButtonCancel->setText( tr( "no" ) ) ;
	this->show() ;
}

void password_dialog::ShowUI()
{
	m_createWallet = true ;
	m_ui->textEdit->setVisible( false ) ;
	m_ui->labelWalletDoesNotExist->setVisible( true ) ;
	m_ui->labelWalletDoesNotExist->setText( tr( "wallet could not be opened with the presented key" ) ) ;
	m_ui->labelHeader->setVisible( false ) ;
	m_ui->lineEditKey->setVisible( false ) ;
	m_ui->pushButtonSend->setVisible( false ) ;
	m_ui->pushButtonCancel->setVisible( false ) ;
	connect( m_ui->pushButtonOK,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
	m_ui->pushButtonOK->setVisible( true ) ;
	this->show() ;
}

void password_dialog::closeUIOnKeySend()
{
	m_closeUIOnKeySend = true ;
}

password_dialog::~password_dialog()
{
	delete m_ui;
}

void password_dialog::pbSend()
{
	if( m_createWallet ){
		emit createWallet( true ) ;
		this->HideUI() ;
	}else{
		emit password( m_ui->lineEditKey->text() ) ;
	}
	if( m_closeUIOnKeySend ){
		this->HideUI() ;
	}
}

void password_dialog::pbCancel()
{
	if( m_createWallet ){
		emit createWallet( false ) ;
		this->HideUI() ;
	}else{
		emit cancelled() ;
		this->HideUI() ;
	}
}

void password_dialog::passwordIsCorrect( bool correctPassword )
{
	if( correctPassword ){
		this->HideUI() ;
	}else{
		m_ui->lineEditKey->clear() ;
		m_ui->lineEditKey->setFocus() ;
		password_dialog * p = new password_dialog() ;
		p->ShowUI() ;
	}
}

void password_dialog::pbOK()
{
	this->HideUI() ;
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
