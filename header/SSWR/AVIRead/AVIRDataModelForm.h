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

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDataModelForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUIListView> lvColumns;
			NotNullPtr<UI::GUIButton> btnPasteData;
			NotNullPtr<UI::GUIButton> btnCancel;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Data::Class> cls;

			static void __stdcall OnPasteDataClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static void __stdcall OnColumnsDblClk(AnyType userObj, UOSInt index);

			void UpdateClassDisp();
		public:
			AVIRDataModelForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDataModelForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
