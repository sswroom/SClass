#ifndef _SM_SSWR_AVIREAD_AVIRCHROMEDEVTOOLSFORM
#define _SM_SSWR_AVIREAD_AVIRCHROMEDEVTOOLSFORM
#include "Net/ChromeDevTools.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

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
			Optional<Net::ChromeDevTools::ChromeProtocol> protocol;

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

			NN<UI::GUITabPage> tpProtocol;
			NN<UI::GUIPanel> pnlProtocol;
			NN<UI::GUILabel> lblProtocolVerMajor;
			NN<UI::GUITextBox> txtProtocolVerMajor;
			NN<UI::GUILabel> lblProtocolVerMinor;
			NN<UI::GUITextBox> txtProtocolVerMinor;
			NN<UI::GUIListBox> lbProtocol;
			NN<UI::GUIHSplitter> hspProtocol;
			NN<UI::GUIPanel> pnlProtocolDomain;
			NN<UI::GUILabel> lblProtocolDomain;
			NN<UI::GUITextBox> txtProtocolDomain;
			NN<UI::GUILabel> lblProtocolDescription;
			NN<UI::GUITextBox> txtProtocolDescription;
			NN<UI::GUICheckBox> chkProtocolDeprecated;
			NN<UI::GUICheckBox> chkProtocolExperimental;
			NN<UI::GUITabControl> tcProtocol;
			NN<UI::GUITabPage> tpProtocolCommands;
			NN<UI::GUIListBox> lbProtocolCommands;
			NN<UI::GUIHSplitter> hspProtocolCommands;
			NN<UI::GUIPanel> pnlProtocolCommands;
			NN<UI::GUILabel> lblProtocolCommandsName;
			NN<UI::GUITextBox> txtProtocolCommandsName;
			NN<UI::GUILabel> lblProtocolCommandsDesc;
			NN<UI::GUITextBox> txtProtocolCommandsDesc;
			NN<UI::GUICheckBox> chkProtocolCommandsDeprecated;
			NN<UI::GUICheckBox> chkProtocolCommandsExperimental;
			NN<UI::GUIGroupBox> grpProtocolCommandsParams;
			NN<UI::GUIListView> lvProtocolCommandsParams;
			NN<UI::GUIVSplitter> vspProtocolCommands;
			NN<UI::GUIGroupBox> grpProtocolCommandsRets;
			NN<UI::GUIListView> lvProtocolCommandsRets;

			NN<UI::GUITabPage> tpProtocolDependencies;
			NN<UI::GUIListBox> lbProtocolDependencies;

			NN<UI::GUITabPage> tpProtocolEvents;
			NN<UI::GUIListBox> lbProtocolEvents;
			NN<UI::GUIHSplitter> hspProtocolEvents;
			NN<UI::GUIPanel> pnlProtocolEvents;
			NN<UI::GUILabel> lblProtocolEventsName;
			NN<UI::GUITextBox> txtProtocolEventsName;
			NN<UI::GUILabel> lblProtocolEventsDesc;
			NN<UI::GUITextBox> txtProtocolEventsDesc;
			NN<UI::GUICheckBox> chkProtocolEventsDeprecated;
			NN<UI::GUICheckBox> chkProtocolEventsExperimental;
			NN<UI::GUIListView> lvProtocolEventsParams;

			NN<UI::GUITabPage> tpProtocolTypes;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnTargetClicked(AnyType userObj);
			static void __stdcall OnTargetSelChg(AnyType userObj);
			static void __stdcall OnProtocolSelChg(AnyType userObj);
			static void __stdcall OnProtocolCommandsSelChg(AnyType userObj);
			static void __stdcall OnProtocolEventsSelChg(AnyType userObj);
			void ReloadTargets();
			void ReloadProtocol();
			static void AppendParameters(NN<UI::GUIListView> lv, NN<const Data::ArrayListNN<Net::ChromeDevTools::ChromeParameter>> params);
			static void AppendParameter(NN<UI::GUIListView> lv, NN<Net::ChromeDevTools::ChromeParameter> param);
		public:
			AVIRChromeDevToolsForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRChromeDevToolsForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
