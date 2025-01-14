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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<IO::Stream> stm;
			Optional<IO::AdvantechASCIIChannel> channel;
			UInt8 channelAddr;
			UInt32 channelModule;

			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUIGroupBox> grpStream;
			NN<UI::GUILabel> lblAddress;
			NN<UI::GUITextBox> txtAddress;
			NN<UI::GUILabel> lblStream;
			NN<UI::GUITextBox> txtStream;
			NN<UI::GUIButton> btnStream;
			NN<UI::GUILabel> lblModuleName;
			NN<UI::GUITextBox> txtModuleName;
			NN<UI::GUILabel> lblFirmware;
			NN<UI::GUITextBox> txtFirmware;
			NN<UI::GUILabel> lblDevAddress;
			NN<UI::GUITextBox> txtDevAddress;
			NN<UI::GUILabel> lblDevBaudRate;
			NN<UI::GUITextBox> txtDevBaudRate;
			NN<UI::GUILabel> lblInputType;
			NN<UI::GUITextBox> txtInputType;
			NN<UI::GUILabel> lblChecksum;
			NN<UI::GUITextBox> txtChecksum;
			NN<UI::GUILabel> lblModbusProtocol;
			NN<UI::GUITextBox> txtModbusProtocol;
			NN<UI::GUILabel> lblDataFormat;
			NN<UI::GUITextBox> txtDataFormat;
			NN<UI::GUILabel> lblIntegrationTime;
			NN<UI::GUITextBox> txtIntegrationTime;
			NN<UI::GUILabel> lblSlewRate;
			NN<UI::GUITextBox> txtSlewRate;
			NN<UI::GUIListView> lvData;

		private:
			static void __stdcall OnStreamClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			void StopStream(Bool clearUI);

		public:
			AVIRADAMForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRADAMForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
