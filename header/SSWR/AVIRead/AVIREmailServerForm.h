#ifndef _SM_SSWR_AVIREAD_AVIREMAILSERVERFORM
#define _SM_SSWR_AVIREAD_AVIREMAILSERVERFORM
#include "Net/Email/EmailStore.h"
#include "Net/Email/POP3Server.h"
#include "Net/Email/SMTPServer.h"
#include "Net/WebServer/GCISNotifyHandler.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/String.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIREmailServerForm : public UI::GUIForm, public Net::Email::MailController
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpSMTP;
			UI::GUIButton *btnSMTPCertKey;
			UI::GUILabel *lblSMTPCertKey;
			UI::GUILabel *lblSMTPPort;
			UI::GUITextBox *txtSMTPPort;
			UI::GUILabel *lblSMTPType;
			UI::GUIComboBox *cboSMTPType;
			UI::GUIButton *btnSMTPStart;
			UI::GUIButton *btnLogFile;

			UI::GUITabPage *tpPOP3;
			UI::GUIButton *btnPOP3CertKey;
			UI::GUILabel *lblPOP3CertKey;
			UI::GUILabel *lblPOP3Port;
			UI::GUITextBox *txtPOP3Port;
			UI::GUICheckBox *chkPOP3SSL;
			UI::GUIButton *btnPOP3Start;

			UI::GUITabPage *tpGCIS;
			UI::GUIButton *btnGCISCertKey;
			UI::GUILabel *lblGCISCertKey;
			UI::GUILabel *lblGCISPort;
			UI::GUITextBox *txtGCISPort;
			UI::GUILabel *lblGCISNotifPath;
			UI::GUITextBox *txtGCISNotifPath;
			UI::GUILabel *lblGCISBatchUplPath;
			UI::GUITextBox *txtGCISBatchUplPath;
			UI::GUIButton *btnGCISStart;

			UI::GUITabPage *tpEmail;
			UI::GUIListView *lvEmail;

			UI::GUITabPage *tpLog;
			UI::GUITextBox *txtLog;
			UI::GUIListBox *lbLog;

			Net::Email::SMTPServer *smtpSvr;
			Net::Email::POP3Server *pop3Svr;
			IO::LogTool log;
			UI::ListBoxLogger *logger;
			Net::SocketFactory *sockf;
			Net::SSLEngine *smtpSSL;
			Crypto::Cert::X509Cert *smtpSSLCert;
			Crypto::Cert::X509File *smtpSSLKey;
			Net::Email::SMTPConn::ConnType smtpType;
			Net::SSLEngine *pop3SSL;
			Crypto::Cert::X509Cert *pop3SSLCert;
			Crypto::Cert::X509File *pop3SSLKey;
			Net::SSLEngine *gcisSSL;
			Crypto::Cert::X509Cert *gcisSSLCert;
			Crypto::Cert::X509File *gcisSSLKey;
			Net::WebServer::WebListener *gcisListener;
			Net::WebServer::GCISNotifyHandler *gcisHdlr;

			Sync::Mutex userMut;
			Data::FastStringMap<UOSInt> userMap;
			Data::ArrayList<Text::String*> userList;

			Net::Email::EmailStore *store;
			UOSInt totalSize;
			UOSInt recvSize;
			Bool mailChanged;

			static void __stdcall OnSMTPStartClicked(void *userObj);
			static void __stdcall OnPOP3StartClicked(void *userObj);
			static void __stdcall OnGCISStartClicked(void *userObj);
			static void __stdcall OnLogFileClicked(void *userObj);
			static void __stdcall OnEmailDblClicked(void *userObj, UOSInt index);
			static UTF8Char *__stdcall OnMailReceived(UTF8Char *queryId, void *userObj, Net::TCPClient *cli, Net::Email::SMTPServer::MailStatus *mail);
			static void __stdcall OnGCISMailReceived(void *userObj, Net::NetConnection *cli, Text::MIMEObj::MailMessage *mail);
			static Bool __stdcall OnMailLogin(void *userObj, Text::CString userName, Text::CString pwd);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnSMTPCertKeyClicked(void *userObj);
			static void __stdcall OnPOP3CertKeyClicked(void *userObj);
			static void __stdcall OnGCISCertKeyClicked(void *userObj);
			static void __stdcall OnSMTPTypeSelChg(void *userObj);
			static void __stdcall OnPOP3SSLChanged(void *userObj, Bool isChecked);

			Text::String *GetUserName(Int32 userId);
		public:
			AVIREmailServerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIREmailServerForm();

			virtual void OnMonitorChanged();

			virtual Bool Login(Text::CString user, Text::CString pwd, Int32 *userId);
			virtual UOSInt GetMessageStat(Int32 userId, UOSInt *size);
			virtual Bool GetUnreadList(Int32 userId, Data::ArrayList<UInt32> *unreadList);
			virtual Bool GetMessageInfo(Int32 userId, UInt32 msgId, MessageInfo *info);
			virtual Bool GetMessageContent(Int32 userId, UInt32 msgId, IO::Stream *stm);
			virtual RemoveStatus RemoveMessage(Int32 userId, UInt32 msgId);
		};
	}
}
#endif
