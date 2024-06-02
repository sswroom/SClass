#ifndef _SM_SSWR_AVIREAD_AVIRUSERAGENTBATCHFORM
#define _SM_SSWR_AVIREAD_AVIRUSERAGENTBATCHFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/CString.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRUserAgentBatchForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlSource;
			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUITextBox> txtSource;
			NN<UI::GUIButton> btnParse;
			NN<UI::GUIButton> btnUpdate;
			NN<UI::GUIButton> btnUpdateCB;
			NN<UI::GUIVSplitter> vspControl;
			NN<UI::GUITextBox> txtOutput;

			static void UserAgent2Output(Text::CStringNN userAgent, NN<Text::StringBuilderUTF8> outSb);
			static void __stdcall OnParseClicked(AnyType userObj);
			static void __stdcall OnUpdateClicked(AnyType userObj);
			static void __stdcall OnUpdateCBClicked(AnyType userObj);
			void UpdateByText(Text::PString txt);
		public:
			AVIRUserAgentBatchForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRUserAgentBatchForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
