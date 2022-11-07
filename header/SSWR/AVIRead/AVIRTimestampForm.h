#ifndef _SM_SSWR_AVIREAD_AVIRTIMESTAMPFORM
#define _SM_SSWR_AVIREAD_AVIRTIMESTAMPFORM
#include "Data/Timestamp.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRTimestampForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblValue;
			UI::GUITextBox *txtValue;
			UI::GUIButton *btnEpochSec;
			UI::GUIButton *btnEpochMS;
			UI::GUIButton *btnEpochUS;
			UI::GUIButton *btnEpochNS;
			UI::GUIButton *btnDotNetTicks;
			UI::GUILabel *lblUTCTime;
			UI::GUITextBox *txtUTCTime;
			UI::GUILabel *lblLocalTime;
			UI::GUITextBox *txtLocalTime;

			SSWR::AVIRead::AVIRCore *core;

			static void __stdcall OnEpochSecClicked(void *userObj);
			static void __stdcall OnEpochMSClicked(void *userObj);
			static void __stdcall OnEpochUSClicked(void *userObj);
			static void __stdcall OnEpochNSClicked(void *userObj);
			static void __stdcall OnDotNetTicksClicked(void *userObj);

			void DisplayTime(Data::Timestamp ts);
		public:
			AVIRTimestampForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRTimestampForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
