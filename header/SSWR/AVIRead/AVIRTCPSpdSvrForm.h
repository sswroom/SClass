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
			SSWR::AVIRead::AVIRCore *core;
			Net::TCPServer *svr;
			Net::TCPClientMgr *cliMgr;
			IO::LogTool *log;

			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUICheckBox *chkMultiThread;
			UI::GUIButton *btnStart;

			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnClientConn(UInt32 *s, void *userObj);
			static void __stdcall OnClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall OnClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
			static void __stdcall OnClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData);
			static UInt32 __stdcall RecvThread(void *userObj);
		public:
			AVIRTCPSpdSvrForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRTCPSpdSvrForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
