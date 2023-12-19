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
			NotNullPtr<UI::GUILabel> lblWFSURL;
			NotNullPtr<UI::GUITextBox> txtWFSURL;
			NotNullPtr<UI::GUILabel> lblWFSVersion;
			NotNullPtr<UI::GUIComboBox> cboWFSVersion;
			NotNullPtr<UI::GUIButton> btnLoad;
			NotNullPtr<UI::GUILabel> lblStatus;
			NotNullPtr<UI::GUITextBox> txtStatus;
			NotNullPtr<UI::GUILabel> lblFeature;
			NotNullPtr<UI::GUIComboBox> cboFeature;
			NotNullPtr<UI::GUIButton> btnOK;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Map::WebFeatureService *wfs;

			static void __stdcall OnLoadClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnFeatureSelChg(void *userObj);
		public:
			AVIRWFSForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWFSForm();

			virtual void OnMonitorChanged();

			Map::MapDrawLayer *LoadLayer();
		};
	}
}
#endif
