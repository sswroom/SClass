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
				const UTF8Char *name;
				UOSInt nameLen;
				UInt8 fPort;
				const UTF8Char *data;
				UOSInt dataLen;
			};
		private:
			static PredefData pdata[];

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Net::SocketFactory> sockf;

			Net::LoRaGateway *lora;
			IO::LogTool log;
			NotNullPtr<UI::ListBoxLogger> logger;

			NotNullPtr<UI::GUITabControl> tcMain;
			
			NotNullPtr<UI::GUITabPage> tpControl;
			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUILabel> lblServerIP;
			NotNullPtr<UI::GUITextBox> txtServerIP;
			NotNullPtr<UI::GUILabel> lblServerPort;
			NotNullPtr<UI::GUITextBox> txtServerPort;
			NotNullPtr<UI::GUILabel> lblGatewayEUI;
			NotNullPtr<UI::GUITextBox> txtGatewayEUI;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUIPanel> pnlDevice;
			NotNullPtr<UI::GUILabel> lblDevAddr;
			NotNullPtr<UI::GUITextBox> txtDevAddr;
			NotNullPtr<UI::GUILabel> lblNwkSKey;
			NotNullPtr<UI::GUITextBox> txtNwkSKey;
			NotNullPtr<UI::GUILabel> lblAppSKey;
			NotNullPtr<UI::GUITextBox> txtAppSKey;
			NotNullPtr<UI::GUILabel> lblFCnt;
			NotNullPtr<UI::GUITextBox> txtFCnt;
			NotNullPtr<UI::GUILabel> lblRSSI;
			NotNullPtr<UI::GUITextBox> txtRSSI;
			NotNullPtr<UI::GUILabel> lblLSNR;
			NotNullPtr<UI::GUITextBox> txtLSNR;
			NotNullPtr<UI::GUILabel> lblPredef;
			NotNullPtr<UI::GUIComboBox> cboPredef;
			NotNullPtr<UI::GUIButton> btnPredef;
			NotNullPtr<UI::GUILabel> lblFPort;
			NotNullPtr<UI::GUITextBox> txtFPort;
			NotNullPtr<UI::GUILabel> lblData;
			NotNullPtr<UI::GUITextBox> txtData;
			NotNullPtr<UI::GUIButton> btnSendULData;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUITextBox> txtLog;
			NotNullPtr<UI::GUIListBox> lbLog;

		private:
			static void __stdcall OnStartClick(AnyType userObj);
			static void __stdcall OnPredefClicked(AnyType userObj);
			static void __stdcall OnSendULDataClick(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
		public:
			AVIRLoraGWSimForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLoraGWSimForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
