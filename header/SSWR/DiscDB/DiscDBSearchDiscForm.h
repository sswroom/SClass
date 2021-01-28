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

			UI::GUIPanel *pnlDiscId;
			UI::GUILabel *lblDiscId;
			UI::GUITextBox *txtDiscId;
			UI::GUIButton *btnSearch;
			UI::GUIPanel *pnlOut;
			UI::GUILabel *lblDiscIdOut;
			UI::GUITextBox *txtDiscIdOut;
			UI::GUILabel *lblDiscType;
			UI::GUITextBox *txtDiscType;
			UI::GUILabel *lblBurntDate;
			UI::GUITextBox *txtBurntDate;
			UI::GUIListView *lvFiles;

			static void __stdcall OnSearchClicked(void *userObj);
		public:
			DiscDBSearchDiscForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::DiscDB::DiscDBEnv *env);
			virtual ~DiscDBSearchDiscForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
