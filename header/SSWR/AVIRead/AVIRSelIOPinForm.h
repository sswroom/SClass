#ifndef _SM_SSWR_AVIREAD_AVIRSELIOPINFORM
#define _SM_SSWR_AVIREAD_AVIRSELIOPINFORM
#include "IO/IOPin.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSelIOPinForm : public UI::GUIForm
		{
		public:
			IO::IOPin *ioPin;
			SSWR::AVIRead::AVIRCore::IOPinType ioPinType;
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::VirtualIOPinMgr *vioPinMgr;
			IO::GPIOControl *gpioPin;

			NotNullPtr<UI::GUIPanel> pnlPinType;
			UI::GUILabel *lblPinType;
			UI::GUIComboBox *cboPinType;
			UI::GUITabControl *tcConfig;

			NotNullPtr<UI::GUITabPage> tpGPIO;
			UI::GUILabel *lblGPIO;
			UI::GUIComboBox *cboGPIO;

			NotNullPtr<UI::GUITabPage> tpVirtualPin;
			UI::GUILabel *lblVirtualPin;
			UI::GUIComboBox *cboVirtualPin;

			NotNullPtr<UI::GUIPanel> pnlButtons;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			static void __stdcall OnOKClick(void *userObj);
			static void __stdcall OnCancelClick(void *userObj);
			static void __stdcall OnPinTypeChg(void *userObj);
		public:
			AVIRSelIOPinForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSelIOPinForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
