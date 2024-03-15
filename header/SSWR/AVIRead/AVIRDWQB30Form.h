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
			NotNullPtr<UI::GUIGroupBox> grpConn;
			NotNullPtr<UI::GUILabel> lblDevType;
			NotNullPtr<UI::GUIComboBox> cboDevType;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUIButton> btnPort;
			NotNullPtr<UI::GUIGroupBox> grpCtrl;
			NotNullPtr<UI::GUILabel> lblMode;
			NotNullPtr<UI::GUITextBox> txtMode;
			NotNullPtr<UI::GUIButton> btnModeScan;
			NotNullPtr<UI::GUIButton> btnModeSetting;
			NotNullPtr<UI::GUIButton> btnModeIdle;
			NotNullPtr<UI::GUIButton> btnReset;
			NotNullPtr<UI::GUIButton> btnDefault;
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpScan;
			NotNullPtr<UI::GUIPanel> pnlScan;
			NotNullPtr<UI::GUILabel> lblScan;
			NotNullPtr<UI::GUITextBox> txtScan;
			NotNullPtr<UI::GUIListBox> lbScan;

			NotNullPtr<UI::GUITabPage> tpSetting;
			NotNullPtr<UI::GUILabel> lblSetCmd;
			NotNullPtr<UI::GUIComboBox> cboSetCmd;
			NotNullPtr<UI::GUIComboBox> cboSetParam;
			NotNullPtr<UI::GUIButton> btnSetCmd;
			NotNullPtr<UI::GUITextBox> txtSetCmd;
			
			IO::CodeScanner::DeviceCommand cmdCurr;
			IO::CodeScanner::CommandType cmdType;
			Int32 cmdMin;
			Int32 cmdMax;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::CodeScanner *scanner;
			Sync::Mutex codeMut;
			Text::String *newCode;
			Bool codeUpdate;

			static void __stdcall OnPortClicked(AnyType userObj);
			static void __stdcall OnModeScanClicked(AnyType userObj);
			static void __stdcall OnModeSettingClicked(AnyType userObj);
			static void __stdcall OnModeIdleClicked(AnyType userObj);
			static void __stdcall OnResetClicked(AnyType userObj);
			static void __stdcall OnDefaultClicked(AnyType userObj);
			static void __stdcall OnSetCmdSelChg(AnyType userObj);
			static void __stdcall OnSetCmdClicked(AnyType userObj);
			static void __stdcall OnCodeScanned(AnyType userObj, Text::CString code);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRDWQB30Form(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDWQB30Form();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
