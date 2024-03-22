#ifndef _SM_SSWR_AVIREAD_AVIRTCPSPDSVRFORM
#define _SM_SSWR_AVIREAD_AVIRTCPSPDSVRFORM
#include "AnyType.h"
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

			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUICheckBox> chkMultiThread;
			NotNullPtr<UI::GUICheckBox> chkEcho;
			NotNullPtr<UI::GUIButton> btnStart;

			static void __stdcall OnStartClick(AnyType userObj);
			static void __stdcall OnClientConn(Socket *s, AnyType userObj);
			static void __stdcall OnClientEvent(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall OnClientData(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
			static void __stdcall OnClientTimeout(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
			static UInt32 __stdcall RecvThread(AnyType userObj);
		public:
			AVIRTCPSpdSvrForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTCPSpdSvrForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
