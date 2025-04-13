#ifndef _SM_SSWR_AVIREAD_AVIRGISCOMBINEFORM
#define _SM_SSWR_AVIREAD_AVIRGISCOMBINEFORM
#include "Map/GPSTrack.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRMapNavigator.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckedListBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPopupMenu.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISCombineForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIButton> btnCheckAll;
			NN<UI::GUIButton> btnUncheckAll;
			NN<UI::GUICheckedListBox> lbLayers;
			NN<UI::GUIButton> btnCombine;
			NN<UI::GUIButton> btnCancel;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Data::ArrayListNN<Map::MapDrawLayer>> layers;
			Data::ArrayListNN<Map::MapDrawLayer> selLayers;

		private:
			static void __stdcall OnCheckAllClick(AnyType userObj);
			static void __stdcall OnUncheckAllClick(AnyType userObj);
			static void __stdcall OnCombineClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);

		public:
			AVIRGISCombineForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::ArrayListNN<Map::MapDrawLayer>> layers);
			virtual ~AVIRGISCombineForm();

			virtual void OnMonitorChanged();

			Optional<Map::MapDrawLayer> GetCombinedLayer();
		};
	}
}
#endif
