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
			Optional<const SSWR::DiscDB::DiscDBEnv::DVDTypeInfo> currRec;

			NotNullPtr<UI::GUILabel> lblID;
			NotNullPtr<UI::GUITextBox> txtID;
			NotNullPtr<UI::GUIButton> btnNew;
			NotNullPtr<UI::GUIButton> btnCancel;
			NotNullPtr<UI::GUILabel> lblName;
			NotNullPtr<UI::GUITextBox> txtName;
			NotNullPtr<UI::GUILabel> lblDescription;
			NotNullPtr<UI::GUITextBox> txtDescription;
			NotNullPtr<UI::GUILabel> lblDisplay;
			NotNullPtr<UI::GUIButton> btnPrev;
			NotNullPtr<UI::GUIButton> btnSave;
			NotNullPtr<UI::GUIButton> btnNext;

			void ShowStatus();
			void UpdateDisplay();
			Bool UpdateRow();

		public:
			DiscDBDVDTypeForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, SSWR::DiscDB::DiscDBEnv *env);
			virtual ~DiscDBDVDTypeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
