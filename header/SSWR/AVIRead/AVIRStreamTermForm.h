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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::SiLabDriver *siLabDriver;

			NotNullPtr<UI::GUIGroupBox> grpStream;
			NotNullPtr<UI::GUILabel> lblStream;
			NotNullPtr<UI::GUITextBox> txtStream;
			NotNullPtr<UI::GUIButton> btnStream;

			NotNullPtr<UI::GUIHSplitter> hspMain;
			NotNullPtr<UI::GUIGroupBox> grpSend;
			NotNullPtr<UI::GUIComboBox> cboSendType;
			NotNullPtr<UI::GUITextBox> txtSendDisp;
			NotNullPtr<UI::GUIPanel> pnlSend;
			NotNullPtr<UI::GUITextBox> txtSendValue;
			NotNullPtr<UI::GUIButton> btnSend;
			NotNullPtr<UI::GUIGroupBox> grpRecv;
			NotNullPtr<UI::GUIComboBox> cboRecvType;
			NotNullPtr<UI::GUITextBox> txtRecvDisp;
			NotNullPtr<UI::GUIPanel> pnlSendOption;
			NotNullPtr<UI::GUILabel> lblSendLBreak;
			NotNullPtr<UI::GUIComboBox> cboSendLBreak;
			NotNullPtr<UI::GUIRadioButton> radSendText;
			NotNullPtr<UI::GUIRadioButton> radSendHex;

			IO::MemoryStream recvBuff;
			IO::MemoryStream sendBuff;
			Sync::Mutex recvMut;
			Bool recvUpdated;

			IO::Stream *stm;
			Bool threadRunning;
			Bool threadToStop;
			Bool remoteClosed;

		private:
			static void __stdcall OnStreamClicked(AnyType userObj);
			static void __stdcall OnSendClicked(AnyType userObj);
			static void __stdcall OnRecvTypeChg(AnyType userObj);
			static void __stdcall OnSendTypeChg(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static UInt32 __stdcall RecvThread(void *userObj);

			void StopStream(Bool clearUI);
			void UpdateRecvDisp();
			void UpdateSendDisp();

		public:
			AVIRStreamTermForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRStreamTermForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
