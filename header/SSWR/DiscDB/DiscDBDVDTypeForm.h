#ifndef _SM_SSWR_DISCDB_DISCDBDVDTYPEFORM
#define _SM_SSWR_DISCDB_DISCDBDVDTYPEFORM
#include "SSWR/DiscDB/DiscDBEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace DiscDB
	{
		class DiscDBDVDTypeForm : public UI::GUIForm
		{
		private:
			SSWR::DiscDB::DiscDBEnv *env;
			UOSInt currIndex;
			Bool newRec;
			const SSWR::DiscDB::DiscDBEnv::DVDTypeInfo *currRec;

			UI::GUILabel *lblID;
			UI::GUITextBox *txtID;
			NotNullPtr<UI::GUIButton> btnNew;
			NotNullPtr<UI::GUIButton> btnCancel;
			UI::GUILabel *lblName;
			UI::GUITextBox *txtName;
			UI::GUILabel *lblDescription;
			UI::GUITextBox *txtDescription;
			UI::GUILabel *lblDisplay;
			NotNullPtr<UI::GUIButton> btnPrev;
			NotNullPtr<UI::GUIButton> btnSave;
			NotNullPtr<UI::GUIButton> btnNext;

			void ShowStatus();
			void UpdateDisplay();
			Bool UpdateRow();

		public:
			DiscDBDVDTypeForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::DiscDB::DiscDBEnv *env);
			virtual ~DiscDBDVDTypeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
