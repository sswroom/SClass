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
			NN<UI::GUILabel> lblWMSURL;
			NN<UI::GUITextBox> txtWMSURL;
			NN<UI::GUILabel> lblWMSVersion;
			NN<UI::GUIComboBox> cboWMSVersion;
			NN<UI::GUIButton> btnLoad;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;
			NN<UI::GUILabel> lblLayer;
			NN<UI::GUIComboBox> cboLayer;
			NN<UI::GUILabel> lblLayerCRS;
			NN<UI::GUIComboBox> cboLayerCRS;
			NN<UI::GUILabel> lblMapImageType;
			NN<UI::GUIComboBox> cboMapImageType;
			NN<UI::GUILabel> lblInfoType;
			NN<UI::GUIComboBox> cboInfoType;
			NN<UI::GUIButton> btnOK;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Map::WebMapService *wms;
			NN<Math::CoordinateSystem> envCsys;

			static void __stdcall OnLoadClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnLayerSelChg(AnyType userObj);
			static void __stdcall OnLayerCRSSelChg(AnyType userObj);
			static void __stdcall OnMapImageTypeSelChg(AnyType userObj);
			static void __stdcall OnInfoTypeSelChg(AnyType userObj);
		public:
			AVIRWMSForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl, NN<Math::CoordinateSystem> envCsys);
			virtual ~AVIRWMSForm();

			virtual void OnMonitorChanged();

			void SetURL(Text::CStringNN url);
			Map::DrawMapService *GetDrawMapService();
		};
	}
}
#endif
