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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			UI::GUIRadioButton *radPredefine;
			UI::GUIRadioButton *radOther;
			NotNullPtr<UI::GUIComboBox> cboPredefine;
			UI::GUITextBox *txtOther;
			UI::GUICheckBox *chkNoResource;
			NotNullPtr<UI::GUILabel> lblSRID;
			UI::GUITextBox *txtSRID;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;
			Map::ESRI::ESRIMapServer *esriMap;

			static void __stdcall OKClicked(void *userObj);
			static void __stdcall CancelClicked(void *userObj);
			static void __stdcall OnOtherChanged(void *userObj);
			static void __stdcall OnNoResourceChg(void *userObj, Bool newValue);
		public:
			AVIRESRIMapForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl);
			virtual ~AVIRESRIMapForm();

			virtual void OnMonitorChanged();
			Map::ESRI::ESRIMapServer *GetSelectedMap();
		};
	}
}
#endif
