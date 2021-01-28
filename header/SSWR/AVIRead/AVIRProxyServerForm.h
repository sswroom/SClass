#ifndef _SM_SSWR_AVIREAD_AVIRPROXYSERVERFORM
#define _SM_SSWR_AVIREAD_AVIRPROXYSERVERFORM

#include "Net/ProxyServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRProxyServerForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SocketFactory *sockf;
			IO::LogTool *log;
			UI::ListBoxLogger *logger;
			Net::ProxyServer *svr;

			UI::GUIPanel *pnlRequest;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUIButton *btnStart;
			UI::GUITextBox *txtLog;
			UI::GUIListBox *lbLog;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnLogSelChg(void *userObj);
		public:
			AVIRProxyServerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRProxyServerForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
