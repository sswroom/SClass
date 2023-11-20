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
			UI::GUILabel *lblStream;
			UI::GUITextBox *txtStream;
			UI::GUIButton *btnStream;

			UI::GUIHSplitter *hspMain;
			NotNullPtr<UI::GUIGroupBox> grpSend;
			UI::GUIComboBox *cboSendType;
			UI::GUITextBox *txtSendDisp;
			NotNullPtr<UI::GUIPanel> pnlSend;
			UI::GUITextBox *txtSendValue;
			UI::GUIButton *btnSend;
			NotNullPtr<UI::GUIGroupBox> grpRecv;
			UI::GUIComboBox *cboRecvType;
			UI::GUITextBox *txtRecvDisp;
			NotNullPtr<UI::GUIPanel> pnlSendOption;
			UI::GUILabel *lblSendLBreak;
			UI::GUIComboBox *cboSendLBreak;
			UI::GUIRadioButton *radSendText;
			UI::GUIRadioButton *radSendHex;

			IO::MemoryStream recvBuff;
			IO::MemoryStream sendBuff;
			Sync::Mutex recvMut;
			Bool recvUpdated;

			IO::Stream *stm;
			Bool threadRunning;
			Bool threadToStop;
			Bool remoteClosed;

		private:
			static void __stdcall OnStreamClicked(void *userObj);
			static void __stdcall OnSendClicked(void *userObj);
			static void __stdcall OnRecvTypeChg(void *userObj);
			static void __stdcall OnSendTypeChg(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static UInt32 __stdcall RecvThread(void *userObj);

			void StopStream(Bool clearUI);
			void UpdateRecvDisp();
			void UpdateSendDisp();

		public:
			AVIRStreamTermForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRStreamTermForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
