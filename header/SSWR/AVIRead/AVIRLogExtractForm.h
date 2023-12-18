#ifndef _SM_SSWR_AVIREAD_AVIRLOGEXTRACTFORM
#define _SM_SSWR_AVIREAD_AVIRLOGEXTRACTFORM
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLogExtractForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			UI::GUILabel *lblSFile;
			UI::GUITextBox *txtSFile;
			NotNullPtr<UI::GUIButton> btnSFile;
			UI::GUILabel *lblPrefix;
			UI::GUITextBox *txtPrefix;
			UI::GUILabel *lblOFile;
			UI::GUITextBox *txtOFile;
			NotNullPtr<UI::GUIButton> btnOFile;
			UI::GUILabel *lblType;
			NotNullPtr<UI::GUIComboBox> cboType;
			UI::GUILabel *lblSuffix;
			UI::GUITextBox *txtSuffix;
			NotNullPtr<UI::GUIButton> btnExtract;

			static void __stdcall OnSFileClicked(void *userObj);
			static void __stdcall OnOFileClicked(void *userObj);
			static void __stdcall OnExtractClicked(void *userObj);
		public:
			AVIRLogExtractForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLogExtractForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
