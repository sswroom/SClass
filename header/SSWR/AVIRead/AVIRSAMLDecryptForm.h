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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUILabel> lblKey;
			NotNullPtr<UI::GUITextBox> txtKey;
			NotNullPtr<UI::GUILabel> lblRAWResponse;
			NotNullPtr<UI::GUITextBox> txtRAWResponse;
			NotNullPtr<UI::GUIButton> btnDecrypt;
			NotNullPtr<UI::GUILabel> lblResult;
			NotNullPtr<UI::GUITextBox> txtResult;

		private:
			static void __stdcall OnFormFiles(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files);
			static void __stdcall OnDecryptClicked(AnyType userObj);
		public:
			AVIRSAMLDecryptForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSAMLDecryptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
