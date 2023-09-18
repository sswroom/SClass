#ifndef _SM_SSWR_AVIREAD_AVIRTCPSPDSVRFORM
#define _SM_SSWR_AVIREAD_AVIRTCPSPDSVRFORM
#include "Net/TCPClientMgr.h"
#include "Net/TCPServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRTCPSpdSvrForm : public UI::GUIForm
		{
		private:
			Bool echo;
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::TCPServer *svr;
			Net::TCPClientMgr *cliMgr;
			IO::LogTool log;

			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUICheckBox *chkMultiThread;
			UI::GUICheckBox *chkEcho;
			UI::GUIButton *btnStart;

			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnClientConn(Socket *s, void *userObj);
			static void __stdcall OnClientEvent(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall OnClientData(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, const Data::ByteArrayR &buff);
			static void __stdcall OnClientTimeout(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData);
			static UInt32 __stdcall RecvThread(void *userObj);
		public:
			AVIRTCPSpdSvrForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTCPSpdSvrForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
