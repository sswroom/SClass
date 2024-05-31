#ifndef _SM_SSWR_AVIREAD_AVIRLORAGWSIMFORM
#define _SM_SSWR_AVIREAD_AVIRLORAGWSIMFORM
#include "Net/LoRaGateway.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIPanel.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLoraGWSimForm : public UI::GUIForm
		{
		private:
			struct PredefData
			{
				UnsafeArray<const UTF8Char> name;
				UOSInt nameLen;
				UInt8 fPort;
				UnsafeArrayOpt<const UTF8Char> data;
				UOSInt dataLen;
			};
		private:
			static PredefData pdata[];

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;

			Net::LoRaGateway *lora;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;

			NN<UI::GUITabControl> tcMain;
			
			NN<UI::GUITabPage> tpControl;
			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblServerIP;
			NN<UI::GUITextBox> txtServerIP;
			NN<UI::GUILabel> lblServerPort;
			NN<UI::GUITextBox> txtServerPort;
			NN<UI::GUILabel> lblGatewayEUI;
			NN<UI::GUITextBox> txtGatewayEUI;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIPanel> pnlDevice;
			NN<UI::GUILabel> lblDevAddr;
			NN<UI::GUITextBox> txtDevAddr;
			NN<UI::GUILabel> lblNwkSKey;
			NN<UI::GUITextBox> txtNwkSKey;
			NN<UI::GUILabel> lblAppSKey;
			NN<UI::GUITextBox> txtAppSKey;
			NN<UI::GUILabel> lblFCnt;
			NN<UI::GUITextBox> txtFCnt;
			NN<UI::GUILabel> lblRSSI;
			NN<UI::GUITextBox> txtRSSI;
			NN<UI::GUILabel> lblLSNR;
			NN<UI::GUITextBox> txtLSNR;
			NN<UI::GUILabel> lblPredef;
			NN<UI::GUIComboBox> cboPredef;
			NN<UI::GUIButton> btnPredef;
			NN<UI::GUILabel> lblFPort;
			NN<UI::GUITextBox> txtFPort;
			NN<UI::GUILabel> lblData;
			NN<UI::GUITextBox> txtData;
			NN<UI::GUIButton> btnSendULData;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIListBox> lbLog;

		private:
			static void __stdcall OnStartClick(AnyType userObj);
			static void __stdcall OnPredefClicked(AnyType userObj);
			static void __stdcall OnSendULDataClick(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
		public:
			AVIRLoraGWSimForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLoraGWSimForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
