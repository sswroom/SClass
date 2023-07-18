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
			SSWR::AVIRead::AVIRCore *core;
			NotNullPtr<Net::SocketFactory> sockf;
			Net::WhoisHandler whois;

			UI::GUIPanel *pnlControl;
			UI::GUILabel *lblSelfIP;
			UI::GUIComboBox *cboSelfIP;
			UI::GUILabel *lblTargetIP;
			UI::GUITextBox *txtTargetIP;
			UI::GUIButton *btnStart;

			UI::GUIListBox *lbIP;
			UI::GUIHSplitter *hspIP;
			UI::GUITextBox *txtIPWhois;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnIPSelChg(void *userObj);

		public:
			AVIRTraceRouteForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRTraceRouteForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
