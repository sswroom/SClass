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
			SSWR::AVIRead::AVIRCore *core;
			IO::Device::RockwellModemController *modem;
			IO::ATCommandChannel *channel;
			IO::Stream *port;

			UI::GUITabControl *tcMain;
			UI::GUITabPage *tpInfo;
			UI::GUILabel *lblModemManu;
			UI::GUITextBox *txtModemManu;
			UI::GUILabel *lblModemModel;
			UI::GUITextBox *txtModemModel;
			UI::GUILabel *lblModemRev;
			UI::GUITextBox *txtModemRev;
			UI::GUILabel *lblModemIMEI;
			UI::GUITextBox *txtModemIMEI;

			UI::GUITabPage *tpDial;
			UI::GUILabel *lblDialNum;
			UI::GUITextBox *txtDialNum;
			UI::GUIButton *btnDial;
			UI::GUIButton *btnHangUp;
			UI::GUILabel *lblDialStatus;
			UI::GUITextBox *txtDialStatus;
			UI::GUILabel *lblDialTones;
			UI::GUITextBox *txtDialTones;

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
			AVIRVoiceModemForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::Device::RockwellModemController *modem, IO::ATCommandChannel *channel, IO::Stream *port);
			virtual ~AVIRVoiceModemForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
