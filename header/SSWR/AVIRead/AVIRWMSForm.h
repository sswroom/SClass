#ifndef _SM_SSWR_AVIREAD_AVIRWMSFORM
#define _SM_SSWR_AVIREAD_AVIRWMSFORM
#include "Map/WebMapService.h"
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
		class AVIRWMSForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUILabel> lblWMSURL;
			NotNullPtr<UI::GUITextBox> txtWMSURL;
			NotNullPtr<UI::GUILabel> lblWMSVersion;
			NotNullPtr<UI::GUIComboBox> cboWMSVersion;
			NotNullPtr<UI::GUIButton> btnLoad;
			NotNullPtr<UI::GUILabel> lblStatus;
			NotNullPtr<UI::GUITextBox> txtStatus;
			NotNullPtr<UI::GUILabel> lblLayer;
			NotNullPtr<UI::GUIComboBox> cboLayer;
			NotNullPtr<UI::GUILabel> lblLayerCRS;
			NotNullPtr<UI::GUIComboBox> cboLayerCRS;
			NotNullPtr<UI::GUILabel> lblMapImageType;
			NotNullPtr<UI::GUIComboBox> cboMapImageType;
			NotNullPtr<UI::GUILabel> lblInfoType;
			NotNullPtr<UI::GUIComboBox> cboInfoType;
			NotNullPtr<UI::GUIButton> btnOK;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Map::WebMapService *wms;
			NotNullPtr<Math::CoordinateSystem> envCsys;

			static void __stdcall OnLoadClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnLayerSelChg(void *userObj);
			static void __stdcall OnLayerCRSSelChg(void *userObj);
			static void __stdcall OnMapImageTypeSelChg(void *userObj);
			static void __stdcall OnInfoTypeSelChg(void *userObj);
		public:
			AVIRWMSForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl, NotNullPtr<Math::CoordinateSystem> envCsys);
			virtual ~AVIRWMSForm();

			virtual void OnMonitorChanged();

			void SetURL(Text::CStringNN url);
			Map::DrawMapService *GetDrawMapService();
		};
	}
}
#endif
