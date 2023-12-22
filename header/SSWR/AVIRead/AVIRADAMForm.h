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

			NotNullPtr<UI::GUIPanel> pnlMain;
			NotNullPtr<UI::GUIGroupBox> grpStream;
			NotNullPtr<UI::GUILabel> lblAddress;
			NotNullPtr<UI::GUITextBox> txtAddress;
			NotNullPtr<UI::GUILabel> lblStream;
			NotNullPtr<UI::GUITextBox> txtStream;
			NotNullPtr<UI::GUIButton> btnStream;
			NotNullPtr<UI::GUILabel> lblModuleName;
			NotNullPtr<UI::GUITextBox> txtModuleName;
			NotNullPtr<UI::GUILabel> lblFirmware;
			NotNullPtr<UI::GUITextBox> txtFirmware;
			NotNullPtr<UI::GUILabel> lblDevAddress;
			NotNullPtr<UI::GUITextBox> txtDevAddress;
			NotNullPtr<UI::GUILabel> lblDevBaudRate;
			NotNullPtr<UI::GUITextBox> txtDevBaudRate;
			NotNullPtr<UI::GUILabel> lblInputType;
			NotNullPtr<UI::GUITextBox> txtInputType;
			NotNullPtr<UI::GUILabel> lblChecksum;
			NotNullPtr<UI::GUITextBox> txtChecksum;
			NotNullPtr<UI::GUILabel> lblModbusProtocol;
			NotNullPtr<UI::GUITextBox> txtModbusProtocol;
			NotNullPtr<UI::GUILabel> lblDataFormat;
			NotNullPtr<UI::GUITextBox> txtDataFormat;
			NotNullPtr<UI::GUILabel> lblIntegrationTime;
			NotNullPtr<UI::GUITextBox> txtIntegrationTime;
			NotNullPtr<UI::GUILabel> lblSlewRate;
			NotNullPtr<UI::GUITextBox> txtSlewRate;
			NotNullPtr<UI::GUIListView> lvData;

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
