#ifndef _SM_SSWR_DISCDB_DISCDBSEARCHDISCFORM
#define _SM_SSWR_DISCDB_DISCDBSEARCHDISCFORM
#include "SSWR/DiscDB/DiscDBEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace DiscDB
	{
		class DiscDBSearchDiscForm : public UI::GUIForm
		{
		private:
			SSWR::DiscDB::DiscDBEnv *env;

			NotNullPtr<UI::GUIPanel> pnlDiscId;
			NotNullPtr<UI::GUILabel> lblDiscId;
			NotNullPtr<UI::GUITextBox> txtDiscId;
			NotNullPtr<UI::GUIButton> btnSearch;
			NotNullPtr<UI::GUIPanel> pnlOut;
			NotNullPtr<UI::GUILabel> lblDiscIdOut;
			NotNullPtr<UI::GUITextBox> txtDiscIdOut;
			NotNullPtr<UI::GUILabel> lblDiscType;
			NotNullPtr<UI::GUITextBox> txtDiscType;
			NotNullPtr<UI::GUILabel> lblBurntDate;
			NotNullPtr<UI::GUITextBox> txtBurntDate;
			NotNullPtr<UI::GUIListView> lvFiles;

			static void __stdcall OnSearchClicked(void *userObj);
		public:
			DiscDBSearchDiscForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::DiscDB::DiscDBEnv *env);
			virtual ~DiscDBSearchDiscForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
