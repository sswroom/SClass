#ifndef _SM_SSWR_AVIREAD_AVIRHIDDEVICEFORM
#define _SM_SSWR_AVIREAD_AVIRHIDDEVICEFORM
#include "IO/HIDInfo.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHIDDeviceForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayList<IO::HIDInfo*> hidList;

			NotNullPtr<UI::GUIListBox> lbDevices;
			NotNullPtr<UI::GUIHSplitter> hspDevices;
			NotNullPtr<UI::GUIPanel> pnlDevices;
			NotNullPtr<UI::GUILabel> lblVendorId;
			NotNullPtr<UI::GUITextBox> txtVendorId;
			NotNullPtr<UI::GUILabel> lblProductId;
			NotNullPtr<UI::GUITextBox> txtProductId;
			NotNullPtr<UI::GUILabel> lblDevPath;
			NotNullPtr<UI::GUITextBox> txtDevPath;
			
			static void __stdcall OnDevicesSelChg(void *userObj);
			static OSInt __stdcall ItemCompare(IO::HIDInfo *item1, IO::HIDInfo *item2);
		public:
			AVIRHIDDeviceForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHIDDeviceForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
