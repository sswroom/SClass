#ifndef _SM_SSWR_AVIREAD_AVIRGISCOMBINEFORM
#define _SM_SSWR_AVIREAD_AVIRGISCOMBINEFORM
#include "Map/GPSTrack.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/IMapNavigator.h"
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
			UI::GUIButton *btnCheckAll;
			UI::GUIButton *btnUncheckAll;
			UI::GUICheckedListBox *lbLayers;
			UI::GUIButton *btnCombine;
			UI::GUIButton *btnCancel;

			SSWR::AVIRead::AVIRCore *core;
			Data::ArrayList<Map::MapDrawLayer*> *layers;
			Data::ArrayList<Map::MapDrawLayer*> *selLayers;

		private:
			static void __stdcall OnCheckAllClick(void *userObj);
			static void __stdcall OnUncheckAllClick(void *userObj);
			static void __stdcall OnCombineClick(void *userObj);
			static void __stdcall OnCancelClick(void *userObj);

		public:
			AVIRGISCombineForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core, Data::ArrayList<Map::MapDrawLayer*> *layers);
			virtual ~AVIRGISCombineForm();

			virtual void OnMonitorChanged();

			Map::MapDrawLayer *GetCombinedLayer();
		};
	};
};
#endif
