#ifndef _SM_SSWR_AVIREAD_AVIRESRIMAPFORM
#define _SM_SSWR_AVIREAD_AVIRESRIMAPFORM
#include "Map/ESRI/ESRIMapServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/String.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRESRIMapForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				const UTF8Char *name;
				UOSInt nameLen;
				const UTF8Char *url;
				UOSInt urlLen;
			} MapServer;
		private:
			static MapServer mapSvrs[];
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			NN<UI::GUIRadioButton> radPredefine;
			NN<UI::GUIRadioButton> radOther;
			NN<UI::GUIComboBox> cboPredefine;
			NN<UI::GUITextBox> txtOther;
			NN<UI::GUICheckBox> chkNoResource;
			NN<UI::GUILabel> lblSRID;
			NN<UI::GUITextBox> txtSRID;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;
			Optional<Map::ESRI::ESRIMapServer> esriMap;

			static void __stdcall OKClicked(AnyType userObj);
			static void __stdcall CancelClicked(AnyType userObj);
			static void __stdcall OnOtherChanged(AnyType userObj);
			static void __stdcall OnNoResourceChg(AnyType userObj, Bool newValue);
		public:
			AVIRESRIMapForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl);
			virtual ~AVIRESRIMapForm();

			virtual void OnMonitorChanged();
			Optional<Map::ESRI::ESRIMapServer> GetSelectedMap();
		};
	}
}
#endif
