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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblUserAgent;
			NN<UI::GUITextBox> txtUserAgent;
			NN<UI::GUIButton> btnParse;
			NN<UI::GUILabel> lblBrowser;
			NN<UI::GUITextBox> txtBrowser;
			NN<UI::GUILabel> lblBrowserVer;
			NN<UI::GUITextBox> txtBrowserVer;
			NN<UI::GUILabel> lblOS;
			NN<UI::GUITextBox> txtOS;
			NN<UI::GUILabel> lblOSVer;
			NN<UI::GUITextBox> txtOSVer;
			NN<UI::GUILabel> lblDeviceName;
			NN<UI::GUITextBox> txtDeviceName;

			static void __stdcall OnParseClicked(AnyType userObj);
		public:
			AVIRUserAgentParseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRUserAgentParseForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
