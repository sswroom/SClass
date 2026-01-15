#ifndef _SM_SSWR_AVIREAD_AVIRGISSHORTESTPATHFORM
#define _SM_SSWR_AVIREAD_AVIRGISSHORTESTPATHFORM
#include "Map/MapDrawLayer.h"
#include "Map/ShortestPath3D.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRMapNavigator.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISShortestPathForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIGroupBox> grpNetwork;
			NN<UI::GUILabel> lblNetwork;
			NN<UI::GUIComboBox> cboNetwork;
			NN<UI::GUIButton> btnNetwork;
			NN<UI::GUIGroupBox> grpPath;
			NN<UI::GUIPanel> pnlPath;
			NN<UI::GUILabel> lblStartPos;
			NN<UI::GUITextBox> txtStartPos;
			NN<UI::GUIButton> btnStartPos;
			NN<UI::GUILabel> lblEndPos;
			NN<UI::GUITextBox> txtEndPos;
			NN<UI::GUIButton> btnEndPos;
			NN<UI::GUIButton> btnSearch;
			NN<UI::GUIListView> lvPaths;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<AVIRMapNavigator> navi;
			NN<Map::MapDrawLayer> layer;
			Map::ShortestPath3D spath;
			UIntOS mode;
			Math::Coord2DDbl startPos;
			Math::Coord2DDbl endPos;
			Data::ArrayListNN<Math::Geometry::LineString> spathLineList;
			NN<Map::ShortestPath3D::PathSession> sess;

			static void __stdcall OnNetworkClicked(AnyType userObj);
			static void __stdcall OnStartPosClicked(AnyType userObj);
			static void __stdcall OnEndPosClicked(AnyType userObj);
			static void __stdcall OnSearchClicked(AnyType userObj);
			static UI::EventState __stdcall OnMouseDown(AnyType userObj, Math::Coord2D<IntOS> scnPos);
			static void __stdcall OnPathsSelChg(AnyType userObj);

			UnsafeArray<UTF8Char> Coord2DDblToString(UnsafeArray<UTF8Char> sbuff, Math::Coord2DDbl coord);
			void UpdatePaths(NN<Data::ArrayListNN<Math::Geometry::LineString>> lineList, NN<Data::ArrayListT<Data::DataArray<Optional<Text::String>>>> propList);
		public:
			AVIRGISShortestPathForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<AVIRMapNavigator> navi, NN<Map::MapDrawLayer> layer);
			virtual ~AVIRGISShortestPathForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
