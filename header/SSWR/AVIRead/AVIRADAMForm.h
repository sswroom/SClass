#ifndef _SM_SSWR_AVIREAD_AVIRADAMFORM
#define _SM_SSWR_AVIREAD_AVIRADAMFORM
#include "IO/AdvantechASCIIChannel.h"
#include "IO/Stream.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRADAMForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::Stream *stm;
			IO::AdvantechASCIIChannel *channel;
			UInt8 channelAddr;
			UInt32 channelModule;

			UI::GUIPanel *pnlMain;
			UI::GUIGroupBox *grpStream;
			UI::GUILabel *lblAddress;
			UI::GUITextBox *txtAddress;
			UI::GUILabel *lblStream;
			UI::GUITextBox *txtStream;
			UI::GUIButton *btnStream;
			UI::GUILabel *lblModuleName;
			UI::GUITextBox *txtModuleName;
			UI::GUILabel *lblFirmware;
			UI::GUITextBox *txtFirmware;
			UI::GUILabel *lblDevAddress;
			UI::GUITextBox *txtDevAddress;
			UI::GUILabel *lblDevBaudRate;
			UI::GUITextBox *txtDevBaudRate;
			UI::GUILabel *lblInputType;
			UI::GUITextBox *txtInputType;
			UI::GUILabel *lblChecksum;
			UI::GUITextBox *txtChecksum;
			UI::GUILabel *lblModbusProtocol;
			UI::GUITextBox *txtModbusProtocol;
			UI::GUILabel *lblDataFormat;
			UI::GUITextBox *txtDataFormat;
			UI::GUILabel *lblIntegrationTime;
			UI::GUITextBox *txtIntegrationTime;
			UI::GUILabel *lblSlewRate;
			UI::GUITextBox *txtSlewRate;
			UI::GUIListView *lvData;

		private:
			static void __stdcall OnStreamClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void StopStream(Bool clearUI);

		public:
			AVIRADAMForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRADAMForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
