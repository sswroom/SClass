#ifndef _SM_SSWR_AVIREAD_AVIRRNCRYPTORFORM
#define _SM_SSWR_AVIREAD_AVIRRNCRYPTORFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRRNCryptorForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUILabel> lblSourceFile;
			NotNullPtr<UI::GUITextBox> txtSourceFile;
			NotNullPtr<UI::GUILabel> lblDestFile;
			NotNullPtr<UI::GUITextBox> txtDestFile;
			NotNullPtr<UI::GUILabel> lblPassword;
			NotNullPtr<UI::GUITextBox> txtPassword;
			NotNullPtr<UI::GUILabel> lblOptions;
			NotNullPtr<UI::GUICheckBox> chkBase64;
			NotNullPtr<UI::GUIButton> btnProcess;

			static void __stdcall OnProcessClicked(void *userObj);
			static void __stdcall OnFiles(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
		public:
			AVIRRNCryptorForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRRNCryptorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
