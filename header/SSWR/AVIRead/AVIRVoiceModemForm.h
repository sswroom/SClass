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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::Device::RockwellModemController> modem;
			NN<IO::ATCommandChannel> channel;
			NN<IO::Stream> port;

			NN<UI::GUITabControl> tcMain;
			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUILabel> lblModemManu;
			NN<UI::GUITextBox> txtModemManu;
			NN<UI::GUILabel> lblModemModel;
			NN<UI::GUITextBox> txtModemModel;
			NN<UI::GUILabel> lblModemRev;
			NN<UI::GUITextBox> txtModemRev;
			NN<UI::GUILabel> lblModemIMEI;
			NN<UI::GUITextBox> txtModemIMEI;

			NN<UI::GUITabPage> tpDial;
			NN<UI::GUILabel> lblDialNum;
			NN<UI::GUITextBox> txtDialNum;
			NN<UI::GUIButton> btnDial;
			NN<UI::GUIButton> btnHangUp;
			NN<UI::GUILabel> lblDialStatus;
			NN<UI::GUITextBox> txtDialStatus;
			NN<UI::GUILabel> lblDialTones;
			NN<UI::GUITextBox> txtDialTones;

			Sync::Mutex toneMut;
			Text::StringBuilderUTF8 toneSb;
			Bool toneChg;

			Bool isConnected;
			Bool hasEvt;
			UInt8 evtType;

			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnDialClicked(AnyType userObj);
			static void __stdcall OnHangUpClicked(AnyType userObj);
			static void __stdcall OnModemEvent(AnyType userObj, UInt8 evtType);

		public:
			AVIRVoiceModemForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::Device::RockwellModemController> modem, NN<IO::ATCommandChannel> channel, NN<IO::Stream> port);
			virtual ~AVIRVoiceModemForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
