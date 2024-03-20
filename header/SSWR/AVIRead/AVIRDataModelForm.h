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
			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUIListView> lvColumns;
			NotNullPtr<UI::GUILabel> lblClassName;
			NotNullPtr<UI::GUITextBox> txtClassName;
			NotNullPtr<UI::GUILabel> lblPrefix;
			NotNullPtr<UI::GUITextBox> txtPrefix;
			NotNullPtr<UI::GUIButton> btnPasteData;
			NotNullPtr<UI::GUIComboBox> cboType;
			NotNullPtr<UI::GUIButton> btnGenerate;
			NotNullPtr<UI::GUILabel> lblStatus;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Data::Class> cls;

			static void __stdcall OnPasteDataClicked(AnyType userObj);
			static void __stdcall OnGenerateClicked(AnyType userObj);
			static void __stdcall OnColumnsDblClk(AnyType userObj, UOSInt index);

			void UpdateClassDisp();
			void PasteData(Bool showError);
		public:
			AVIRDataModelForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDataModelForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
