#ifndef _SM_SSWR_AVIREAD_AVIRTIMESTAMPFORM
#define _SM_SSWR_AVIREAD_AVIRTIMESTAMPFORM
#include "Data/Timestamp.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRTimestampForm : public UI::GUIForm
		{
		private:
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpValue;
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
			
			NotNullPtr<UI::GUITabPage> tpStr;
			UI::GUILabel *lblStr;
			UI::GUITextBox *txtStr;
			UI::GUIButton *btnStrConv;
			UI::GUILabel *lblStrEpochSec;
			UI::GUITextBox *txtStrEpochSec;
			UI::GUILabel *lblStrEpochMS;
			UI::GUITextBox *txtStrEpochMS;
			UI::GUILabel *lblStrEpochNS;
			UI::GUITextBox *txtStrEpochNS;
			UI::GUILabel *lblStrDotNetTicks;
			UI::GUITextBox *txtStrDotNetTicks;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnEpochSecClicked(void *userObj);
			static void __stdcall OnEpochMSClicked(void *userObj);
			static void __stdcall OnEpochUSClicked(void *userObj);
			static void __stdcall OnEpochNSClicked(void *userObj);
			static void __stdcall OnDotNetTicksClicked(void *userObj);
			static void __stdcall OnStrConvClicked(void *userObj);
			void DisplayTime(const Data::Timestamp &ts);
		public:
			AVIRTimestampForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTimestampForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
