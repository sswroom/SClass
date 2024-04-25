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
			NN<SSWR::AVIRead::AVIRCore> core;
			IO::VirtualIOPinMgr *vioPinMgr;
			IO::GPIOControl *gpioPin;

			NN<UI::GUIPanel> pnlPinType;
			NN<UI::GUILabel> lblPinType;
			NN<UI::GUIComboBox> cboPinType;
			NN<UI::GUITabControl> tcConfig;

			NN<UI::GUITabPage> tpGPIO;
			NN<UI::GUILabel> lblGPIO;
			NN<UI::GUIComboBox> cboGPIO;

			NN<UI::GUITabPage> tpVirtualPin;
			NN<UI::GUILabel> lblVirtualPin;
			NN<UI::GUIComboBox> cboVirtualPin;

			NN<UI::GUIPanel> pnlButtons;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);
			static void __stdcall OnPinTypeChg(AnyType userObj);
		public:
			AVIRSelIOPinForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSelIOPinForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
