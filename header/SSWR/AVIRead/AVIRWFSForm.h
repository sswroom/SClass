#ifndef _SM_SSWR_AVIREAD_AVIRWFSFORM
#define _SM_SSWR_AVIREAD_AVIRWFSFORM
#include "Map/WebFeatureService.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWFSForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblWFSURL;
			UI::GUITextBox *txtWFSURL;
			UI::GUILabel *lblWFSVersion;
			UI::GUIComboBox *cboWFSVersion;
			UI::GUIButton *btnLoad;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;
			UI::GUILabel *lblFeature;
			UI::GUIComboBox *cboFeature;
			UI::GUIButton *btnOK;

			SSWR::AVIRead::AVIRCore *core;
			Map::WebFeatureService *wfs;

			static void __stdcall OnLoadClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnFeatureSelChg(void *userObj);
		public:
			AVIRWFSForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRWFSForm();

			virtual void OnMonitorChanged();

			Map::IMapDrawLayer *LoadLayer();
		};
	}
}
#endif
