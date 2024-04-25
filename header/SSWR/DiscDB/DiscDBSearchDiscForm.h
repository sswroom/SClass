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

			NN<UI::GUIPanel> pnlDiscId;
			NN<UI::GUILabel> lblDiscId;
			NN<UI::GUITextBox> txtDiscId;
			NN<UI::GUIButton> btnSearch;
			NN<UI::GUIPanel> pnlOut;
			NN<UI::GUILabel> lblDiscIdOut;
			NN<UI::GUITextBox> txtDiscIdOut;
			NN<UI::GUILabel> lblDiscType;
			NN<UI::GUITextBox> txtDiscType;
			NN<UI::GUILabel> lblBurntDate;
			NN<UI::GUITextBox> txtBurntDate;
			NN<UI::GUIListView> lvFiles;

			static void __stdcall OnSearchClicked(AnyType userObj);
		public:
			DiscDBSearchDiscForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, SSWR::DiscDB::DiscDBEnv *env);
			virtual ~DiscDBSearchDiscForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
