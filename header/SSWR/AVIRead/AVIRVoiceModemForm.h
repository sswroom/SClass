#ifndef _SM_SSWR_AVIREAD_AVIRVOICEMODEMFORM
#define _SM_SSWR_AVIREAD_AVIRVOICEMODEMFORM
#include "IO/Device/RockwellModemController.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRVoiceModemForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::Device::RockwellModemController *modem;
			IO::ATCommandChannel *channel;
			NotNullPtr<IO::Stream> port;

			NotNullPtr<UI::GUITabControl> tcMain;
			NotNullPtr<UI::GUITabPage> tpInfo;
			NotNullPtr<UI::GUILabel> lblModemManu;
			NotNullPtr<UI::GUITextBox> txtModemManu;
			NotNullPtr<UI::GUILabel> lblModemModel;
			NotNullPtr<UI::GUITextBox> txtModemModel;
			NotNullPtr<UI::GUILabel> lblModemRev;
			NotNullPtr<UI::GUITextBox> txtModemRev;
			NotNullPtr<UI::GUILabel> lblModemIMEI;
			NotNullPtr<UI::GUITextBox> txtModemIMEI;

			NotNullPtr<UI::GUITabPage> tpDial;
			NotNullPtr<UI::GUILabel> lblDialNum;
			NotNullPtr<UI::GUITextBox> txtDialNum;
			NotNullPtr<UI::GUIButton> btnDial;
			NotNullPtr<UI::GUIButton> btnHangUp;
			NotNullPtr<UI::GUILabel> lblDialStatus;
			NotNullPtr<UI::GUITextBox> txtDialStatus;
			NotNullPtr<UI::GUILabel> lblDialTones;
			NotNullPtr<UI::GUITextBox> txtDialTones;

			Sync::Mutex toneMut;
			Text::StringBuilderUTF8 toneSb;
			Bool toneChg;

			Bool isConnected;
			Bool hasEvt;
			UInt8 evtType;

			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnDialClicked(void *userObj);
			static void __stdcall OnHangUpClicked(void *userObj);
			static void __stdcall OnModemEvent(void *userObj, UInt8 evtType);

		public:
			AVIRVoiceModemForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::Device::RockwellModemController *modem, IO::ATCommandChannel *channel, NotNullPtr<IO::Stream> port);
			virtual ~AVIRVoiceModemForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
