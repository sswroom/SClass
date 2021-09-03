#ifndef _SM_SSWR_AVIREAD_AVIRSMTPSERVERFORM
#define _SM_SSWR_AVIREAD_AVIRSMTPSERVERFORM
#include "Net/Email/POP3Server.h"
#include "Net/Email/SMTPServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
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
		class AVIRSMTPServerForm : public UI::GUIForm, public Net::Email::MailController
		{
		private:
			typedef struct
			{
				Int64 id;
				Net::SocketUtil::AddressInfo remoteAddr;
				UInt16 remotePort;
				Int64 recvTime;
				const UTF8Char *fromAddr;
				Data::ArrayList<const UTF8Char *> *rcptList;
				const UTF8Char *fileName;
				const UTF8Char *uid;
				UOSInt fileSize;
				Bool isDeleted;
			} EmailInfo;
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUITabControl *tcMain;
			UI::GUITabPage *tpControl;
			UI::GUITabPage *tpEmail;
			UI::GUITabPage *tpLog;

			UI::GUIButton *btnCertKey;
			UI::GUILabel *lblCertKey;
			UI::GUIGroupBox *grpSMTP;
			UI::GUILabel *lblSMTPPort;
			UI::GUITextBox *txtSMTPPort;
			UI::GUILabel *lblSMTPType;
			UI::GUIComboBox *cboSMTPType;
			UI::GUIButton *btnSMTPStart;
			UI::GUIGroupBox *grpPOP3;
			UI::GUILabel *lblPOP3Port;
			UI::GUITextBox *txtPOP3Port;
			UI::GUICheckBox *chkPOP3SSL;
			UI::GUIButton *btnPOP3Start;
			UI::GUIButton *btnLogFile;

			UI::GUIListView *lvEmail;

			UI::GUITextBox *txtLog;
			UI::GUIListBox *lbLog;

			Net::Email::SMTPServer *smtpSvr;
			Net::Email::POP3Server *pop3Svr;
			IO::LogTool *log;
			UI::ListBoxLogger *logger;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Crypto::Cert::X509File *sslCert;
			Crypto::Cert::X509File *sslKey;
			Net::Email::SMTPConn::ConnType smtpType;

			Int64 currId;
			Sync::Mutex *currIdMut;

			Data::ArrayList<EmailInfo*> *mailList;
			Sync::Mutex *mailMut;
			UOSInt totalSize;
			UOSInt recvIndex;
			UOSInt recvSize;
			Bool mailChanged;

			static void __stdcall OnSMTPStartClicked(void *userObj);
			static void __stdcall OnPOP3StartClicked(void *userObj);
			static void __stdcall OnLogFileClicked(void *userObj);
			static void __stdcall OnEmailDblClicked(void *userObj, UOSInt index);
			static UTF8Char *__stdcall OnMailReceived(UTF8Char *queryId, void *userObj, Net::TCPClient *cli, Net::Email::SMTPServer::MailStatus *mail);
			static Bool __stdcall OnMailLogin(void *userObj, const UTF8Char *userName, const UTF8Char *pwd);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnCertKeyClicked(void *userObj);
			static void __stdcall OnSMTPTypeSelChg(void *userObj);
			static void __stdcall OnPOP3SSLChanged(void *userObj, Bool isChecked);
			Int64 NextEmailId();
		public:
			AVIRSMTPServerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRSMTPServerForm();

			virtual void OnMonitorChanged();

			virtual Bool Login(const Char *user, const Char *pwd, Int32 *userId);
			virtual UOSInt GetMessageStat(Int32 userId, UOSInt *size);
			virtual Bool GetUnreadList(Int32 userId, Data::ArrayList<UInt32> *unreadList);
			virtual Bool GetMessageInfo(Int32 userId, UInt32 msgId, MessageInfo *info);
			virtual Bool GetMessageContent(Int32 userId, UInt32 msgId, IO::Stream *stm);
			virtual RemoveStatus RemoveMessage(Int32 userId, UInt32 msgId);
		};
	}
}
#endif
