#ifndef PASSWORD_DIALOG_H
#define PASSWORD_DIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QString>
#include <QMessageBox>

namespace Ui {
class password_dialog;
}

class password_dialog : public QDialog
{
	Q_OBJECT
public:
	explicit password_dialog( QWidget *parent = 0 ) ;
	void ShowUI( const QString& walletName,const QString& applicationName ) ;
	void ShowUI( const QString& walletName ) ;
	void ShowUI( void ) ;
	void closeUIOnKeySend( void ) ;
	~password_dialog();
signals:
	void cancelled( void ) ;
	void password( QString ) ;
	void createWallet( bool ) ;
private slots:
	void pbSend( void ) ;
	void pbCancel( void ) ;
	void passwordIsCorrect( bool ) ;
	void pbOK( void ) ;
private:
	void HideUI( void ) ;
	void closeEvent( QCloseEvent * ) ;
	Ui::password_dialog * m_ui ;
	bool m_createWallet ;
	bool m_closeUIOnKeySend ;
};

#endif // PASSWORD_DIALOG_H
