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
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpValue;
			NN<UI::GUILabel> lblValue;
			NN<UI::GUITextBox> txtValue;
			NN<UI::GUIButton> btnEpochSec;
			NN<UI::GUIButton> btnEpochMS;
			NN<UI::GUIButton> btnEpochUS;
			NN<UI::GUIButton> btnEpochNS;
			NN<UI::GUIButton> btnDotNetTicks;
			NN<UI::GUILabel> lblUTCTime;
			NN<UI::GUITextBox> txtUTCTime;
			NN<UI::GUILabel> lblLocalTime;
			NN<UI::GUITextBox> txtLocalTime;
			
			NN<UI::GUITabPage> tpStr;
			NN<UI::GUILabel> lblStr;
			NN<UI::GUITextBox> txtStr;
			NN<UI::GUIButton> btnStrConv;
			NN<UI::GUILabel> lblStrEpochSec;
			NN<UI::GUITextBox> txtStrEpochSec;
			NN<UI::GUILabel> lblStrEpochMS;
			NN<UI::GUITextBox> txtStrEpochMS;
			NN<UI::GUILabel> lblStrEpochNS;
			NN<UI::GUITextBox> txtStrEpochNS;
			NN<UI::GUILabel> lblStrDotNetTicks;
			NN<UI::GUITextBox> txtStrDotNetTicks;

			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnEpochSecClicked(AnyType userObj);
			static void __stdcall OnEpochMSClicked(AnyType userObj);
			static void __stdcall OnEpochUSClicked(AnyType userObj);
			static void __stdcall OnEpochNSClicked(AnyType userObj);
			static void __stdcall OnDotNetTicksClicked(AnyType userObj);
			static void __stdcall OnStrConvClicked(AnyType userObj);
			void DisplayTime(const Data::Timestamp &ts);
		public:
			AVIRTimestampForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTimestampForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
