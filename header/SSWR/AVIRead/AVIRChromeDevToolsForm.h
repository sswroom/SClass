#ifndef _SM_SSWR_AVIREAD_AVIRCHROMEDEVTOOLSFORM
#define _SM_SSWR_AVIREAD_AVIRCHROMEDEVTOOLSFORM
#include "Net/ChromeDevTools.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRChromeDevToolsForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Optional<Net::ChromeDevTools> devTools;
			Optional<Net::ChromeDevTools::ChromeTargets> targets;

			NN<UI::GUIPanel> pnlPort;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpVersion;
			NN<UI::GUILabel> lblVersionBrowser;
			NN<UI::GUITextBox> txtVersionBrowser;
			NN<UI::GUILabel> lblVersionProtocol;
			NN<UI::GUITextBox> txtVersionProtocol;
			NN<UI::GUILabel> lblVersionUserAgent;
			NN<UI::GUITextBox> txtVersionUserAgent;
			NN<UI::GUILabel> lblVersionV8;
			NN<UI::GUITextBox> txtVersionV8;
			NN<UI::GUILabel> lblVersionWebKit;
			NN<UI::GUITextBox> txtVersionWebKit;
			NN<UI::GUILabel> lblVersionWSDebuggerUrl;
			NN<UI::GUITextBox> txtVersionWSDebuggerUrl;

			NN<UI::GUITabPage> tpTarget;
			NN<UI::GUIPanel> pnlTarget;
			NN<UI::GUIButton> btnTarget;
			NN<UI::GUIListBox> lbTarget;
			NN<UI::GUIPanel> pnlTargetDetail;
			NN<UI::GUILabel> lblTargetDescription;
			NN<UI::GUITextBox> txtTargetDescription;
			NN<UI::GUILabel> lblTargetDevToolsFrontendUrl;
			NN<UI::GUITextBox> txtTargetDevToolsFrontendUrl;
			NN<UI::GUILabel> lblTargetId;
			NN<UI::GUITextBox> txtTargetId;
			NN<UI::GUILabel> lblTargetTitle;
			NN<UI::GUITextBox> txtTargetTitle;
			NN<UI::GUILabel> lblTargetType;
			NN<UI::GUITextBox> txtTargetType;
			NN<UI::GUILabel> lblTargetURL;
			NN<UI::GUITextBox> txtTargetURL;
			NN<UI::GUILabel> lblTargetWebSocketDebuggerUrl;
			NN<UI::GUITextBox> txtTargetWebSocketDebuggerUrl;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnTargetClicked(AnyType userObj);
			static void __stdcall OnTargetSelChg(AnyType userObj);
			void ReloadTargets();
		public:
			AVIRChromeDevToolsForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRChromeDevToolsForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
