#ifndef _SM_SSWR_AVIREAD_AVIRJTT808SERVERFORM
#define _SM_SSWR_AVIREAD_AVIRJTT808SERVERFORM
#include "IO/ProtoHdlr/ProtoJTT808Handler.h"
#include "Net/TCPClientMgr.h"
#include "Net/TCPServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRJTT808ServerForm : public UI::GUIForm, public IO::IProtocolHandler::DataListener
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			IO::LogTool log;
			Net::TCPServer *svr;
			Net::TCPClientMgr *cliMgr;
			IO::ProtoHdlr::ProtoJTT808Handler *protoHdlr;
			NN<UI::ListBoxLogger> logger;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpStatus;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUIButton> btnStart;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
			static void __stdcall OnClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall OnClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
			static void __stdcall OnClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
			static void __stdcall OnClientConn(NN<Socket> s, AnyType userObj);

			void ServerStop();
		public:
			AVIRJTT808ServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRJTT808ServerForm();

			virtual void OnMonitorChanged();

			virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize);
			virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UOSInt buffSize);
		};
	}
}
#endif
