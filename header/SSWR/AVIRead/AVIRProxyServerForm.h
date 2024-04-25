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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;
			Net::ProxyServer *svr;

			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIListBox> lbLog;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
		public:
			AVIRProxyServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRProxyServerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
