#ifndef _SM_SSWR_AVIREAD_AVIRSTREAMTERMFORM
#define _SM_SSWR_AVIREAD_AVIRSTREAMTERMFORM
#include "IO/MemoryStream.h"
#include "IO/SiLabDriver.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRStreamTermForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<IO::SiLabDriver> siLabDriver;

			NN<UI::GUIGroupBox> grpStream;
			NN<UI::GUILabel> lblStream;
			NN<UI::GUITextBox> txtStream;
			NN<UI::GUIButton> btnStream;

			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUIGroupBox> grpSend;
			NN<UI::GUIComboBox> cboSendType;
			NN<UI::GUITextBox> txtSendDisp;
			NN<UI::GUIPanel> pnlSend;
			NN<UI::GUITextBox> txtSendValue;
			NN<UI::GUIButton> btnSend;
			NN<UI::GUIGroupBox> grpRecv;
			NN<UI::GUIComboBox> cboRecvType;
			NN<UI::GUITextBox> txtRecvDisp;
			NN<UI::GUIPanel> pnlSendOption;
			NN<UI::GUILabel> lblSendLBreak;
			NN<UI::GUIComboBox> cboSendLBreak;
			NN<UI::GUIRadioButton> radSendText;
			NN<UI::GUIRadioButton> radSendHex;

			IO::MemoryStream recvBuff;
			IO::MemoryStream sendBuff;
			Sync::Mutex recvMut;
			Bool recvUpdated;

			Optional<IO::Stream> stm;
			Bool threadRunning;
			Bool threadToStop;
			Bool remoteClosed;

		private:
			static void __stdcall OnStreamClicked(AnyType userObj);
			static void __stdcall OnSendClicked(AnyType userObj);
			static void __stdcall OnRecvTypeChg(AnyType userObj);
			static void __stdcall OnSendTypeChg(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static UInt32 __stdcall RecvThread(AnyType userObj);

			void StopStream(Bool clearUI);
			void UpdateRecvDisp();
			void UpdateSendDisp();

		public:
			AVIRStreamTermForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRStreamTermForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
