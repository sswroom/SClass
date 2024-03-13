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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUILabel> lblUserAgent;
			NotNullPtr<UI::GUITextBox> txtUserAgent;
			NotNullPtr<UI::GUIButton> btnParse;
			NotNullPtr<UI::GUILabel> lblBrowser;
			NotNullPtr<UI::GUITextBox> txtBrowser;
			NotNullPtr<UI::GUILabel> lblBrowserVer;
			NotNullPtr<UI::GUITextBox> txtBrowserVer;
			NotNullPtr<UI::GUILabel> lblOS;
			NotNullPtr<UI::GUITextBox> txtOS;
			NotNullPtr<UI::GUILabel> lblOSVer;
			NotNullPtr<UI::GUITextBox> txtOSVer;
			NotNullPtr<UI::GUILabel> lblDeviceName;
			NotNullPtr<UI::GUITextBox> txtDeviceName;

			static void __stdcall OnParseClicked(void *userObj);
		public:
			AVIRUserAgentParseForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRUserAgentParseForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
