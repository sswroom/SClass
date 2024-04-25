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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblSourceFile;
			NN<UI::GUITextBox> txtSourceFile;
			NN<UI::GUILabel> lblDestFile;
			NN<UI::GUITextBox> txtDestFile;
			NN<UI::GUILabel> lblPassword;
			NN<UI::GUITextBox> txtPassword;
			NN<UI::GUILabel> lblOptions;
			NN<UI::GUICheckBox> chkBase64;
			NN<UI::GUIButton> btnProcess;

			static void __stdcall OnProcessClicked(AnyType userObj);
			static void __stdcall OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files);
		public:
			AVIRRNCryptorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRRNCryptorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
