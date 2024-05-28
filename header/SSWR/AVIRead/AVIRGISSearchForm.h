#ifndef _SM_SSWR_AVIREAD_AVIRGISSEARCHFORM
#define _SM_SSWR_AVIREAD_AVIRGISSEARCHFORM
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListStrUTF8.h"
#include "Map/MapDrawLayer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/IMapNavigator.h"
#include "Text/SearchIndexer.h"
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISSearchForm : public UI::GUIForm
		{
		private:
			NN<UI::GUITextBox> txtSearchStr;
			NN<UI::GUIListBox> lbResults;

			NN<SSWR::AVIRead::AVIRCore> core;
			IMapNavigator *navi;
			NN<Map::MapDrawLayer> layer;
			NN<Text::SearchIndexer> searching;
			Data::ArrayListString dispList;
			Data::ArrayListInt64 objIds;
			Map::NameArray *nameArr;
			UOSInt strIndex;
			Int32 flags;

		private:
			static void __stdcall OnTextChg(AnyType userObj);
			static void __stdcall OnResultSelChg(AnyType userObj);
//			System::Void txtSearchStr_KeyPress(System::Object *  sender, System::Windows::Forms::KeyPressEventArgs *  e);
			void UpdateResults();

		public:
			AVIRGISSearchForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, IMapNavigator *navi, NN<Map::MapDrawLayer> layer, NN<Text::SearchIndexer> searching, UOSInt strIndex, Int32 flags);
			virtual ~AVIRGISSearchForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
