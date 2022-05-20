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

			SSWR::AVIRead::AVIRCore *core;
			Net::SocketFactory *sockf;

			Net::LoRaGateway *lora;
			IO::LogTool log;
			UI::ListBoxLogger *logger;

			UI::GUITabControl *tcMain;
			
			UI::GUITabPage *tpControl;
			UI::GUIPanel *pnlControl;
			UI::GUILabel *lblServerIP;
			UI::GUITextBox *txtServerIP;
			UI::GUILabel *lblServerPort;
			UI::GUITextBox *txtServerPort;
			UI::GUILabel *lblGatewayEUI;
			UI::GUITextBox *txtGatewayEUI;
			UI::GUIButton *btnStart;
			UI::GUIPanel *pnlDevice;
			UI::GUILabel *lblDevAddr;
			UI::GUITextBox *txtDevAddr;
			UI::GUILabel *lblNwkSKey;
			UI::GUITextBox *txtNwkSKey;
			UI::GUILabel *lblAppSKey;
			UI::GUITextBox *txtAppSKey;
			UI::GUILabel *lblFCnt;
			UI::GUITextBox *txtFCnt;
			UI::GUILabel *lblRSSI;
			UI::GUITextBox *txtRSSI;
			UI::GUILabel *lblLSNR;
			UI::GUITextBox *txtLSNR;
			UI::GUILabel *lblPredef;
			UI::GUIComboBox *cboPredef;
			UI::GUIButton *btnPredef;
			UI::GUILabel *lblFPort;
			UI::GUITextBox *txtFPort;
			UI::GUILabel *lblData;
			UI::GUITextBox *txtData;
			UI::GUIButton *btnSendULData;

			UI::GUITabPage *tpLog;
			UI::GUITextBox *txtLog;
			UI::GUIListBox *lbLog;

		private:
			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnPredefClicked(void *userObj);
			static void __stdcall OnSendULDataClick(void *userObj);
			static void __stdcall OnLogSelChg(void *userObj);
		public:
			AVIRLoraGWSimForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRLoraGWSimForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
