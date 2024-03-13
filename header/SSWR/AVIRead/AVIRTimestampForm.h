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
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpValue;
			NotNullPtr<UI::GUILabel> lblValue;
			NotNullPtr<UI::GUITextBox> txtValue;
			NotNullPtr<UI::GUIButton> btnEpochSec;
			NotNullPtr<UI::GUIButton> btnEpochMS;
			NotNullPtr<UI::GUIButton> btnEpochUS;
			NotNullPtr<UI::GUIButton> btnEpochNS;
			NotNullPtr<UI::GUIButton> btnDotNetTicks;
			NotNullPtr<UI::GUILabel> lblUTCTime;
			NotNullPtr<UI::GUITextBox> txtUTCTime;
			NotNullPtr<UI::GUILabel> lblLocalTime;
			NotNullPtr<UI::GUITextBox> txtLocalTime;
			
			NotNullPtr<UI::GUITabPage> tpStr;
			NotNullPtr<UI::GUILabel> lblStr;
			NotNullPtr<UI::GUITextBox> txtStr;
			NotNullPtr<UI::GUIButton> btnStrConv;
			NotNullPtr<UI::GUILabel> lblStrEpochSec;
			NotNullPtr<UI::GUITextBox> txtStrEpochSec;
			NotNullPtr<UI::GUILabel> lblStrEpochMS;
			NotNullPtr<UI::GUITextBox> txtStrEpochMS;
			NotNullPtr<UI::GUILabel> lblStrEpochNS;
			NotNullPtr<UI::GUITextBox> txtStrEpochNS;
			NotNullPtr<UI::GUILabel> lblStrDotNetTicks;
			NotNullPtr<UI::GUITextBox> txtStrDotNetTicks;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnEpochSecClicked(void *userObj);
			static void __stdcall OnEpochMSClicked(void *userObj);
			static void __stdcall OnEpochUSClicked(void *userObj);
			static void __stdcall OnEpochNSClicked(void *userObj);
			static void __stdcall OnDotNetTicksClicked(void *userObj);
			static void __stdcall OnStrConvClicked(void *userObj);
			void DisplayTime(const Data::Timestamp &ts);
		public:
			AVIRTimestampForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTimestampForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
