#ifndef _SM_SSWR_AVIREAD_AVIRSOLAREDGEFORM
#define _SM_SSWR_AVIREAD_AVIRSOLAREDGEFORM
#include "Net/SolarEdgeAPI.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSolarEdgeForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			Net::SolarEdgeAPI *seAPI;

			UI::GUIPanel *pnlAPIKey;
			UI::GUILabel *lblAPIKey;
			UI::GUITextBox *txtAPIKey;
			UI::GUIButton *btnAPIKey;
			UI::GUITabControl *tcMain;
			
			UI::GUITabPage *tpVersion;
			UI::GUILabel *lblCurrVer;
			UI::GUITextBox *txtCurrVer;
			UI::GUILabel *lblSuppVer;
			UI::GUITextBox *txtSuppVer;

			static void __stdcall OnAPIKeyClicked(void *userObj);
		public:
			AVIRSolarEdgeForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRSolarEdgeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
