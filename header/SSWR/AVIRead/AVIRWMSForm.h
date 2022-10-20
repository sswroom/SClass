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
			UI::GUILabel *lblWMSURL;
			UI::GUITextBox *txtWMSURL;
			UI::GUILabel *lblWMSVersion;
			UI::GUIComboBox *cboWMSVersion;
			UI::GUIButton *btnLoad;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;
			UI::GUILabel *lblLayer;
			UI::GUIComboBox *cboLayer;
			UI::GUILabel *lblLayerCRS;
			UI::GUIComboBox *cboLayerCRS;
			UI::GUILabel *lblMapImageType;
			UI::GUIComboBox *cboMapImageType;
			UI::GUILabel *lblInfoType;
			UI::GUIComboBox *cboInfoType;
			UI::GUIButton *btnOK;

			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			Map::WebMapService *wms;

			static void __stdcall OnLoadClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnLayerSelChg(void *userObj);
			static void __stdcall OnLayerCRSSelChg(void *userObj);
			static void __stdcall OnMapImageTypeSelChg(void *userObj);
			static void __stdcall OnInfoTypeSelChg(void *userObj);
		public:
			AVIRWMSForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Net::SSLEngine *ssl);
			virtual ~AVIRWMSForm();

			virtual void OnMonitorChanged();

			Map::DrawMapService *GetDrawMapService();
		};
	}
}
#endif
