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
			SSWR::AVIRead::AVIRCore *core;
			IO::LogTool *log;
			Net::TCPServer *svr;
			Net::TCPClientMgr *cliMgr;
			IO::ProtoHdlr::ProtoJTT808Handler *protoHdlr;
			UI::ListBoxLogger *logger;

			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpStatus;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUIButton *btnStart;

			UI::GUITabPage *tpLog;
			UI::GUIListBox *lbLog;
			UI::GUITextBox *txtLog;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnLogSelChg(void *userObj);
			static void __stdcall OnClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall OnClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
			static void __stdcall OnClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData);
			static void __stdcall OnClientConn(Socket *s, void *userObj);

			void ServerStop();
		public:
			AVIRJTT808ServerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRJTT808ServerForm();

			virtual void OnMonitorChanged();

			virtual void DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
			virtual void DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize);
		};
	}
}
#endif
