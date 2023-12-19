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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::LogTool log;
			Net::TCPServer *svr;
			Net::TCPClientMgr *cliMgr;
			IO::ProtoHdlr::ProtoJTT808Handler *protoHdlr;
			NotNullPtr<UI::ListBoxLogger> logger;

			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpStatus;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUIButton> btnStart;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			NotNullPtr<UI::GUITextBox> txtLog;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnLogSelChg(void *userObj);
			static void __stdcall OnClientEvent(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall OnClientData(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, const Data::ByteArrayR &buff);
			static void __stdcall OnClientTimeout(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData);
			static void __stdcall OnClientConn(Socket *s, void *userObj);

			void ServerStop();
		public:
			AVIRJTT808ServerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRJTT808ServerForm();

			virtual void OnMonitorChanged();

			virtual void DataParsed(NotNullPtr<IO::Stream> stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
			virtual void DataSkipped(NotNullPtr<IO::Stream> stm, void *stmObj, const UInt8 *buff, UOSInt buffSize);
		};
	}
}
#endif
