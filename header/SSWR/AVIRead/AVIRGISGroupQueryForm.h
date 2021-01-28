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
			UI::GUITextBox *txtLayer;
			UI::GUIListView *lvInfo;

			SSWR::AVIRead::AVIRCore *core;
			IMapNavigator *navi;
			Map::MapEnv *env;
			Map::MapEnv::GroupItem *group;
			OSInt downX;
			OSInt downY;

			static Bool __stdcall OnMouseDown(void *userObj, OSInt x, OSInt y);
			static Bool __stdcall OnMouseUp(void *userObj, OSInt x, OSInt y);
		public:
			AVIRGISGroupQueryForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IMapNavigator *navi, Map::MapEnv *env, Map::MapEnv::GroupItem *group);
			virtual ~AVIRGISGroupQueryForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
