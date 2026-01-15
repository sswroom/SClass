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
			UIntOS currIndex;
			Bool newRec;
			Optional<const SSWR::DiscDB::DiscDBEnv::DVDTypeInfo> currRec;

			NN<UI::GUILabel> lblID;
			NN<UI::GUITextBox> txtID;
			NN<UI::GUIButton> btnNew;
			NN<UI::GUIButton> btnCancel;
			NN<UI::GUILabel> lblName;
			NN<UI::GUITextBox> txtName;
			NN<UI::GUILabel> lblDescription;
			NN<UI::GUITextBox> txtDescription;
			NN<UI::GUILabel> lblDisplay;
			NN<UI::GUIButton> btnPrev;
			NN<UI::GUIButton> btnSave;
			NN<UI::GUIButton> btnNext;

			void ShowStatus();
			void UpdateDisplay();
			Bool UpdateRow();

		public:
			DiscDBDVDTypeForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, SSWR::DiscDB::DiscDBEnv *env);
			virtual ~DiscDBDVDTypeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
