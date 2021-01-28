#ifndef _SM_SSWR_AVIREAD_AVIRDWQB30FORM
#define _SM_SSWR_AVIREAD_AVIRDWQB30FORM
#include "IO/CodeScanner.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Mutex.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDWQB30Form : public UI::GUIForm
		{
		private:
			UI::GUIGroupBox *grpConn;
			UI::GUILabel *lblDevType;
			UI::GUIComboBox *cboDevType;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUIButton *btnPort;
			UI::GUIGroupBox *grpCtrl;
			UI::GUILabel *lblMode;
			UI::GUITextBox *txtMode;
			UI::GUIButton *btnModeScan;
			UI::GUIButton *btnModeSetting;
			UI::GUIButton *btnModeIdle;
			UI::GUIButton *btnReset;
			UI::GUIButton *btnDefault;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpScan;
			UI::GUIPanel *pnlScan;
			UI::GUILabel *lblScan;
			UI::GUITextBox *txtScan;
			UI::GUIListBox *lbScan;

			UI::GUITabPage *tpSetting;
			UI::GUILabel *lblSetCmd;
			UI::GUIComboBox *cboSetCmd;
			UI::GUIComboBox *cboSetParam;
			UI::GUIButton *btnSetCmd;
			UI::GUITextBox *txtSetCmd;
			
			IO::CodeScanner::DeviceCommand cmdCurr;
			IO::CodeScanner::CommandType cmdType;
			Int32 cmdMin;
			Int32 cmdMax;

			SSWR::AVIRead::AVIRCore *core;
			IO::CodeScanner *scanner;
			Sync::Mutex *codeMut;
			const UTF8Char *newCode;
			Bool codeUpdate;

			static void __stdcall OnPortClicked(void *userObj);
			static void __stdcall OnModeScanClicked(void *userObj);
			static void __stdcall OnModeSettingClicked(void *userObj);
			static void __stdcall OnModeIdleClicked(void *userObj);
			static void __stdcall OnResetClicked(void *userObj);
			static void __stdcall OnDefaultClicked(void *userObj);
			static void __stdcall OnSetCmdSelChg(void *userObj);
			static void __stdcall OnSetCmdClicked(void *userObj);
			static void __stdcall OnCodeScanned(void *userObj, const UTF8Char *code);
			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRDWQB30Form(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRDWQB30Form();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
