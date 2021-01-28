#ifndef _SM_SSWR_AVIREAD_AVIRUSERAGENTPARSEFORM
#define _SM_SSWR_AVIREAD_AVIRUSERAGENTPARSEFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRUserAgentParseForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUILabel *lblUserAgent;
			UI::GUITextBox *txtUserAgent;
			UI::GUIButton *btnParse;
			UI::GUILabel *lblBrowser;
			UI::GUITextBox *txtBrowser;
			UI::GUILabel *lblBrowserVer;
			UI::GUITextBox *txtBrowserVer;
			UI::GUILabel *lblOS;
			UI::GUITextBox *txtOS;
			UI::GUILabel *lblOSVer;
			UI::GUITextBox *txtOSVer;
			UI::GUILabel *lblDeviceName;
			UI::GUITextBox *txtDeviceName;

			static void __stdcall OnParseClicked(void *userObj);
		public:
			AVIRUserAgentParseForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRUserAgentParseForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
