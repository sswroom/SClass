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
			NN<UI::GUILabel> lblWFSURL;
			NN<UI::GUITextBox> txtWFSURL;
			NN<UI::GUILabel> lblWFSVersion;
			NN<UI::GUIComboBox> cboWFSVersion;
			NN<UI::GUIButton> btnLoad;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;
			NN<UI::GUILabel> lblFeature;
			NN<UI::GUIComboBox> cboFeature;
			NN<UI::GUIButton> btnOK;

			NN<SSWR::AVIRead::AVIRCore> core;
			Map::WebFeatureService *wfs;

			static void __stdcall OnLoadClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnFeatureSelChg(AnyType userObj);
		public:
			AVIRWFSForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWFSForm();

			virtual void OnMonitorChanged();

			Optional<Map::MapDrawLayer> LoadLayer();
		};
	}
}
#endif
