#ifndef _SM_SSWR_AVIREAD_AVIRGISQUERYFORM
#define _SM_SSWR_AVIREAD_AVIRGISQUERYFORM
#include "Map/VectorLayer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/IMapNavigator.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISQueryForm : public UI::GUIForm
		{
		private:

			UI::GUIListView *lvInfo;

			SSWR::AVIRead::AVIRCore *core;
			IMapNavigator *navi;
			Map::IMapDrawLayer *lyr;
			OSInt downX;
			OSInt downY;

			static Bool __stdcall OnMouseDown(void *userObj, OSInt x, OSInt y);
			static Bool __stdcall OnMouseUp(void *userObj, OSInt x, OSInt y);
		public:
			AVIRGISQueryForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::IMapDrawLayer *lyr, IMapNavigator *navi);
			virtual ~AVIRGISQueryForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
