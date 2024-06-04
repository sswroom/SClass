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
			NN<UI::GUIGroupBox> grpConn;
			NN<UI::GUILabel> lblDevType;
			NN<UI::GUIComboBox> cboDevType;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUIButton> btnPort;
			NN<UI::GUIGroupBox> grpCtrl;
			NN<UI::GUILabel> lblMode;
			NN<UI::GUITextBox> txtMode;
			NN<UI::GUIButton> btnModeScan;
			NN<UI::GUIButton> btnModeSetting;
			NN<UI::GUIButton> btnModeIdle;
			NN<UI::GUIButton> btnReset;
			NN<UI::GUIButton> btnDefault;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpScan;
			NN<UI::GUIPanel> pnlScan;
			NN<UI::GUILabel> lblScan;
			NN<UI::GUITextBox> txtScan;
			NN<UI::GUIListBox> lbScan;

			NN<UI::GUITabPage> tpSetting;
			NN<UI::GUILabel> lblSetCmd;
			NN<UI::GUIComboBox> cboSetCmd;
			NN<UI::GUIComboBox> cboSetParam;
			NN<UI::GUIButton> btnSetCmd;
			NN<UI::GUITextBox> txtSetCmd;
			
			IO::CodeScanner::DeviceCommand cmdCurr;
			IO::CodeScanner::CommandType cmdType;
			Int32 cmdMin;
			Int32 cmdMax;

			NN<SSWR::AVIRead::AVIRCore> core;
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
			static void __stdcall OnCodeScanned(AnyType userObj, Text::CStringNN code);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRDWQB30Form(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDWQB30Form();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
