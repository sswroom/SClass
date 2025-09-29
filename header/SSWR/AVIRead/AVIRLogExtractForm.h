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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::GUILabel> lblSFile;
			NN<UI::GUITextBox> txtSFile;
			NN<UI::GUIButton> btnSFile;
			NN<UI::GUIComboBox> cboCompare;
			NN<UI::GUITextBox> txtCompare;
			NN<UI::GUILabel> lblOFile;
			NN<UI::GUITextBox> txtOFile;
			NN<UI::GUIButton> btnOFile;
			NN<UI::GUILabel> lblOutType;
			NN<UI::GUIComboBox> cboOutType;
			NN<UI::GUILabel> lblSuffix;
			NN<UI::GUITextBox> txtSuffix;
			NN<UI::GUIButton> btnExtract;

			static void __stdcall OnSFileClicked(AnyType userObj);
			static void __stdcall OnOFileClicked(AnyType userObj);
			static void __stdcall OnExtractClicked(AnyType userObj);
		public:
			AVIRLogExtractForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLogExtractForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
