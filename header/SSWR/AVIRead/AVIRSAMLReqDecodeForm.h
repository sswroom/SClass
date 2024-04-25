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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblRAWRequest;
			NN<UI::GUITextBox> txtRAWRequest;
			NN<UI::GUIButton> btnDecode;
			NN<UI::GUILabel> lblResult;
			NN<UI::GUITextBox> txtResult;
			NN<UI::GUILabel> lblWellFormat;
			NN<UI::GUITextBox> txtWellFormat;

		private:
			static void __stdcall OnDecodeClicked(AnyType userObj);
		public:
			AVIRSAMLReqDecodeForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSAMLReqDecodeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
