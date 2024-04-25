#ifndef _SM_SSWR_AVIREAD_AVIRSAMLDECRYPTFORM
#define _SM_SSWR_AVIREAD_AVIRSAMLDECRYPTFORM
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
		class AVIRSAMLDecryptForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblKey;
			NN<UI::GUITextBox> txtKey;
			NN<UI::GUILabel> lblRAWResponse;
			NN<UI::GUITextBox> txtRAWResponse;
			NN<UI::GUIButton> btnDecrypt;
			NN<UI::GUILabel> lblResult;
			NN<UI::GUITextBox> txtResult;

		private:
			static void __stdcall OnFormFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnDecryptClicked(AnyType userObj);
		public:
			AVIRSAMLDecryptForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSAMLDecryptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
