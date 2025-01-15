#ifndef _SM_SSWR_AVIREAD_AVIRGISGROUPQUERYFORM
#define _SM_SSWR_AVIREAD_AVIRGISGROUPQUERYFORM
#include "Map/MapEnv.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/IMapNavigator.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISGroupQueryForm : public UI::GUIForm
		{
		private:
			NN<UI::GUITextBox> txtLayer;
			NN<UI::GUIListView> lvInfo;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IMapNavigator> navi;
			NN<Map::MapEnv> env;
			Optional<Map::MapEnv::GroupItem> group;
			Math::Coord2D<OSInt> downPos;

			static Bool __stdcall OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos);
		public:
			AVIRGISGroupQueryForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IMapNavigator> navi, NN<Map::MapEnv> env, Optional<Map::MapEnv::GroupItem> group);
			virtual ~AVIRGISGroupQueryForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
