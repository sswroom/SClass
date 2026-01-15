#ifndef _SM_SSWR_AVIREAD_AVIRGISGROUPQUERYFORM
#define _SM_SSWR_AVIREAD_AVIRGISGROUPQUERYFORM
#include "Data/ArrayListNative.hpp"
#include "Map/MapEnv.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRMapNavigator.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISGroupQueryForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIComboBox> cboItem;
			NN<UI::GUIListView> lvInfo;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<AVIRMapNavigator> navi;
			NN<Map::MapEnv> env;
			Optional<Map::MapEnv::GroupItem> group;
			Math::Coord2D<OSInt> downPos;

			Data::ArrayListNN<Math::Geometry::Vector2D> queryVecList;
			Data::ArrayListNN<Math::Geometry::Vector2D> queryVecOriList;
			Data::ArrayListNative<UOSInt> queryValueOfstList;
			Data::ArrayListStringNN queryNameList;
			Data::ArrayListNN<Text::String> queryValueList;

			static UI::EventState __stdcall OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static UI::EventState __stdcall OnMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static void __stdcall OnItemSelChg(AnyType userObj);
			void ClearQueryResults();
			void SetQueryItem(UOSInt index);
		public:
			AVIRGISGroupQueryForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<AVIRMapNavigator> navi, NN<Map::MapEnv> env, Optional<Map::MapEnv::GroupItem> group);
			virtual ~AVIRGISGroupQueryForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
