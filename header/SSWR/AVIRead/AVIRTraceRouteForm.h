#ifndef _SM_SSWR_AVIREAD_AVIRTRACEROUTEFORM
#define _SM_SSWR_AVIREAD_AVIRTRACEROUTEFORM

#include "Net/WhoisHandler.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRTraceRouteForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			Net::WhoisHandler whois;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblSelfIP;
			NN<UI::GUIComboBox> cboSelfIP;
			NN<UI::GUILabel> lblTargetIP;
			NN<UI::GUITextBox> txtTargetIP;
			NN<UI::GUIButton> btnStart;

			NN<UI::GUIListBox> lbIP;
			NN<UI::GUIHSplitter> hspIP;
			NN<UI::GUITextBox> txtIPWhois;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnIPSelChg(AnyType userObj);

		public:
			AVIRTraceRouteForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTraceRouteForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
