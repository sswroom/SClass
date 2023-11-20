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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIPanel> pnlSource;
			NotNullPtr<UI::GUIPanel> pnlControl;
			UI::GUITextBox *txtSource;
			UI::GUIButton *btnParse;
			UI::GUIButton *btnUpdate;
			UI::GUIButton *btnUpdateCB;
			UI::GUIVSplitter *vspControl;
			UI::GUITextBox *txtOutput;

			static void UserAgent2Output(Text::CString userAgent, NotNullPtr<Text::StringBuilderUTF8> outSb);
			static void __stdcall OnParseClicked(void *userObj);
			static void __stdcall OnUpdateClicked(void *userObj);
			static void __stdcall OnUpdateCBClicked(void *userObj);
			void UpdateByText(Text::PString txt);
		public:
			AVIRUserAgentBatchForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRUserAgentBatchForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
