#ifndef _SM_SSWR_AVIREAD_AVIRGISOSMDATAFORM
#define _SM_SSWR_AVIREAD_AVIRGISOSMDATAFORM
#include "Map/OSM/OSMData.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRMapNavigator.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
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
			Bool mouseDown;
			Math::Coord2D<IntOS> mouseDownPos;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUICheckBox> chkShowUnknown;
			NN<UI::GUIButton> btnDefaultStyle;
			NN<UI::GUIButton> btnCenterline;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpRelation;
			NN<UI::GUIPanel> pnlRelation;
			NN<UI::GUILabel> lblRelationType;
			NN<UI::GUIComboBox> cboRelationType;
			NN<UI::GUIListBox> lbRelation;
			NN<UI::GUIListView> lvRelationMember;
			NN<UI::GUIListView> lvRelationTags;

			NN<UI::GUITabPage> tpQuery;
			NN<UI::GUIListBox> lbQueryResult;
			NN<UI::GUIPanel> pnlQuery;
			NN<UI::GUILabel> lblQueryType;
			NN<UI::GUITextBox> txtQueryType;
			NN<UI::GUIListView> lvQueryTags;

			NN<UI::GUITabPage> tpRestriction;
			NN<UI::GUIListView> lvRestriction;
		private:
			static void __stdcall OnRelationSelChg(AnyType userObj);
			static void __stdcall OnRelationTypeSelChg(AnyType userObj);
			static void __stdcall OnQueryResultSelChg(AnyType userObj);
			static UI::EventState __stdcall OnMouseLDown(AnyType userObj, Math::Coord2D<IntOS> scnPos);
			static UI::EventState __stdcall OnMouseLUp(AnyType userObj, Math::Coord2D<IntOS> scnPos);
			static void __stdcall OnShowUnknownChg(AnyType userObj, Bool newState);
			static void __stdcall OnDefaultStyleClicked(AnyType userObj);
			static void __stdcall OnCenterlineClicked(AnyType userObj);
			void UpdateRelationList();
		public:
			AVIRGISOSMDataForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::OSM::OSMData> osmData, NN<SSWR::AVIRead::AVIRMapNavigator> nav);
			virtual ~AVIRGISOSMDataForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
