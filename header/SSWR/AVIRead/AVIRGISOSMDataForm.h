#ifndef _SM_SSWR_AVIREAD_AVIRGISFORM
#define _SM_SSWR_AVIREAD_AVIRGISFORM
#include "Map/OSM/OSMData.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRMapNavigator.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIMainMenu.h"
#include "UI/GUIMapControl.h"
#include "UI/GUIMapTreeView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBox.h"
#include "UI/GUIPopupMenu.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUITrackBar.h"


namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISOSMDataForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Map::OSM::OSMData> osmData;
			NN<SSWR::AVIRead::AVIRMapNavigator> nav;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpRelation;
			NN<UI::GUIListBox> lbRelation;
			NN<UI::GUIListView> lvRelation;

		private:
		public:
			AVIRGISOSMDataForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::OSM::OSMData> osmData, NN<SSWR::AVIRead::AVIRMapNavigator> nav);
			virtual ~AVIRGISOSMDataForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
