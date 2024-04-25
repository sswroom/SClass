#ifndef _SM_SSWR_AVIREAD_AVIRNTPCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRNTPCLIENTFORM
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
		class AVIRNTPClientForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblServer;
			NN<UI::GUIComboBox> cboServer;
			NN<UI::GUILabel> lblServerTime;
			NN<UI::GUITextBox> txtServerTime;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;
			NN<UI::GUIButton> btnGetTime;
			NN<UI::GUIButton> btnSyncTime;

			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnGetTimeClicked(AnyType userObj);
			static void __stdcall OnSyncTimeClicked(AnyType userObj);
		public:
			AVIRNTPClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRNTPClientForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
