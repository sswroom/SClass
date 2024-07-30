#ifndef _SM_SSWR_AVIREAD_AVIREMAILSERVERFORM
#define _SM_SSWR_AVIREAD_AVIREMAILSERVERFORM
#include "Data/ArrayListNN.h"
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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpSMTP;
			NN<UI::GUIButton> btnSMTPCertKey;
			NN<UI::GUILabel> lblSMTPCertKey;
			NN<UI::GUILabel> lblSMTPPort;
			NN<UI::GUITextBox> txtSMTPPort;
			NN<UI::GUILabel> lblSMTPType;
			NN<UI::GUIComboBox> cboSMTPType;
			NN<UI::GUIButton> btnSMTPStart;
			NN<UI::GUIButton> btnLogFile;

			NN<UI::GUITabPage> tpPOP3;
			NN<UI::GUIButton> btnPOP3CertKey;
			NN<UI::GUILabel> lblPOP3CertKey;
			NN<UI::GUILabel> lblPOP3Port;
			NN<UI::GUITextBox> txtPOP3Port;
			NN<UI::GUICheckBox> chkPOP3SSL;
			NN<UI::GUIButton> btnPOP3Start;

			NN<UI::GUITabPage> tpGCIS;
			NN<UI::GUIButton> btnGCISCertKey;
			NN<UI::GUILabel> lblGCISCertKey;
			NN<UI::GUILabel> lblGCISPort;
			NN<UI::GUITextBox> txtGCISPort;
			NN<UI::GUILabel> lblGCISNotifPath;
			NN<UI::GUITextBox> txtGCISNotifPath;
			NN<UI::GUILabel> lblGCISBatchUplPath;
			NN<UI::GUITextBox> txtGCISBatchUplPath;
			NN<UI::GUIButton> btnGCISStart;

			NN<UI::GUITabPage> tpEmail;
			NN<UI::GUIListView> lvEmail;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIListBox> lbLog;

			Net::Email::SMTPServer *smtpSvr;
			Net::Email::POP3Server *pop3Svr;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> smtpSSL;
			Optional<Crypto::Cert::X509Cert> smtpSSLCert;
			Optional<Crypto::Cert::X509File> smtpSSLKey;
			Data::ArrayListNN<Crypto::Cert::X509Cert> smtpCACerts;
			Net::Email::SMTPConn::ConnType smtpType;
			Optional<Net::SSLEngine> pop3SSL;
			Optional<Crypto::Cert::X509Cert> pop3SSLCert;
			Optional<Crypto::Cert::X509File> pop3SSLKey;
			Data::ArrayListNN<Crypto::Cert::X509Cert> pop3CACerts;
			Optional<Net::SSLEngine> gcisSSL;
			Optional<Crypto::Cert::X509Cert> gcisSSLCert;
			Optional<Crypto::Cert::X509File> gcisSSLKey;
			Data::ArrayListNN<Crypto::Cert::X509Cert> gcisCACerts;
			Net::WebServer::WebListener *gcisListener;
			Net::WebServer::GCISNotifyHandler *gcisHdlr;

			Sync::Mutex userMut;
			Data::FastStringMap<UOSInt> userMap;
			Data::ArrayListStringNN userList;

			Net::Email::EmailStore *store;
			UOSInt totalSize;
			UOSInt recvSize;
			Bool mailChanged;

			static void __stdcall OnSMTPStartClicked(AnyType userObj);
			static void __stdcall OnPOP3StartClicked(AnyType userObj);
			static void __stdcall OnGCISStartClicked(AnyType userObj);
			static void __stdcall OnLogFileClicked(AnyType userObj);
			static void __stdcall OnEmailDblClicked(AnyType userObj, UOSInt index);
			static UnsafeArrayOpt<UTF8Char> __stdcall OnMailReceived(UnsafeArray<UTF8Char> queryId, AnyType userObj, NN<Net::TCPClient> cli, NN<const Net::Email::SMTPServer::MailStatus> mail);
			static void __stdcall OnGCISMailReceived(AnyType userObj, NN<Net::NetConnection> cli, NN<const Text::MIMEObj::MailMessage> mail);
			static Bool __stdcall OnMailLogin(AnyType userObj, Text::CStringNN userName, Text::CStringNN pwd);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnSMTPCertKeyClicked(AnyType userObj);
			static void __stdcall OnPOP3CertKeyClicked(AnyType userObj);
			static void __stdcall OnGCISCertKeyClicked(AnyType userObj);
			static void __stdcall OnSMTPTypeSelChg(AnyType userObj);
			static void __stdcall OnPOP3SSLChanged(AnyType userObj, Bool isChecked);

			Optional<Text::String> GetUserName(Int32 userId);
			void ClearSMTPCACerts();
			void ClearPOP3CACerts();
			void ClearGCISCACerts();
		public:
			AVIREmailServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREmailServerForm();

			virtual void OnMonitorChanged();

			virtual Bool Login(Text::CStringNN user, Text::CStringNN pwd, OutParam<Int32> userId);
			virtual UOSInt GetMessageStat(Int32 userId, UOSInt *size);
			virtual Bool GetUnreadList(Int32 userId, Data::ArrayList<UInt32> *unreadList);
			virtual Bool GetMessageInfo(Int32 userId, UInt32 msgId, MessageInfo *info);
			virtual Bool GetMessageContent(Int32 userId, UInt32 msgId, IO::Stream *stm);
			virtual RemoveStatus RemoveMessage(Int32 userId, UInt32 msgId);
		};
	}
}
#endif
