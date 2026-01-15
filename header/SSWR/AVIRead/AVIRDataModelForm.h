#ifndef _SM_SSWR_AVIREAD_AVIRDATAMODELFORM
#define _SM_SSWR_AVIREAD_AVIRDATAMODELFORM
#include "Data/Class.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
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
		class AVIRDataModelForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIListView> lvColumns;
			NN<UI::GUILabel> lblClassName;
			NN<UI::GUITextBox> txtClassName;
			NN<UI::GUILabel> lblPrefix;
			NN<UI::GUITextBox> txtPrefix;
			NN<UI::GUIButton> btnPasteData;
			NN<UI::GUIComboBox> cboType;
			NN<UI::GUIButton> btnGenerate;
			NN<UI::GUILabel> lblStatus;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Data::Class> cls;

			static void __stdcall OnPasteDataClicked(AnyType userObj);
			static void __stdcall OnGenerateClicked(AnyType userObj);
			static void __stdcall OnColumnsDblClk(AnyType userObj, UIntOS index);

			void UpdateClassDisp();
			void PasteData(Bool showError);
		public:
			AVIRDataModelForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDataModelForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
