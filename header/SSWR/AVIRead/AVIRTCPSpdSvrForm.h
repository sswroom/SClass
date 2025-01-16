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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::TCPServer> svr;
			Optional<Net::TCPClientMgr> cliMgr;
			IO::LogTool log;

			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUICheckBox> chkMultiThread;
			NN<UI::GUICheckBox> chkEcho;
			NN<UI::GUIButton> btnStart;

			static void __stdcall OnStartClick(AnyType userObj);
			static void __stdcall OnClientConn(NN<Socket> s, AnyType userObj);
			static void __stdcall OnClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall OnClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
			static void __stdcall OnClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
			static UInt32 __stdcall RecvThread(AnyType userObj);
		public:
			AVIRTCPSpdSvrForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTCPSpdSvrForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
