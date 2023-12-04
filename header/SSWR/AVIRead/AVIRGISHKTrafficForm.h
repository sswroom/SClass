#ifndef _SM_SSWR_AVIREAD_AVIRGISHKTRAFFICFORM
#define _SM_SSWR_AVIREAD_AVIRGISHKTRAFFICFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISHKTrafficForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			UI::GUILabel *lblRoadCenterline;
			UI::GUITextBox *txtRoadCenterline;
			UI::GUIButton *btnRoadCenterline;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			Map::MapDrawLayer *lyr;

			static void __stdcall OnRoadCenterlineClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRGISHKTrafficForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGISHKTrafficForm();

			virtual void OnMonitorChanged();

			Map::MapDrawLayer *GetMapLayer();
		};
	};
};
#endif
