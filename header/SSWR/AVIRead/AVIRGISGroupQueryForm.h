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
			NotNullPtr<UI::GUITextBox> txtLayer;
			NotNullPtr<UI::GUIListView> lvInfo;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IMapNavigator *navi;
			NotNullPtr<Map::MapEnv> env;
			Map::MapEnv::GroupItem *group;
			Math::Coord2D<OSInt> downPos;

			static Bool __stdcall OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos);
		public:
			AVIRGISGroupQueryForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IMapNavigator *navi, NotNullPtr<Map::MapEnv> env, Map::MapEnv::GroupItem *group);
			virtual ~AVIRGISGroupQueryForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
