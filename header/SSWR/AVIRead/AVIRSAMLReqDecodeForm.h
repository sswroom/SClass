#ifndef _SM_SSWR_AVIREAD_AVIRSAMLREQDECODEFORM
#define _SM_SSWR_AVIREAD_AVIRSAMLREQDECODEFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSAMLReqDecodeForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUILabel> lblRAWRequest;
			NotNullPtr<UI::GUITextBox> txtRAWRequest;
			NotNullPtr<UI::GUIButton> btnDecode;
			NotNullPtr<UI::GUILabel> lblResult;
			NotNullPtr<UI::GUITextBox> txtResult;
			NotNullPtr<UI::GUILabel> lblWellFormat;
			NotNullPtr<UI::GUITextBox> txtWellFormat;

		private:
			static void __stdcall OnDecodeClicked(void *userObj);
		public:
			AVIRSAMLReqDecodeForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSAMLReqDecodeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
