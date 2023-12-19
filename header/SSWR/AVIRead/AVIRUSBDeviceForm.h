#ifndef _SM_SSWR_AVIREAD_AVIRUSBDEVICEFORM
#define _SM_SSWR_AVIREAD_AVIRUSBDEVICEFORM
#include "IO/USBInfo.h"
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
		class AVIRUSBDeviceForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayList<IO::USBInfo*> usbList;

			UI::GUIListBox *lbDevices;
			NotNullPtr<UI::GUIHSplitter> hspDevices;
			NotNullPtr<UI::GUIPanel> pnlDevices;
			NotNullPtr<UI::GUILabel> lblVendorId;
			NotNullPtr<UI::GUITextBox> txtVendorId;
			NotNullPtr<UI::GUILabel> lblVendorName;
			NotNullPtr<UI::GUITextBox> txtVendorName;
			NotNullPtr<UI::GUILabel> lblProductId;
			NotNullPtr<UI::GUITextBox> txtProductId;
			NotNullPtr<UI::GUILabel> lblDevice;
			NotNullPtr<UI::GUITextBox> txtDevice;
			NotNullPtr<UI::GUILabel> lblDispName;
			NotNullPtr<UI::GUITextBox> txtDispName;
			NotNullPtr<UI::GUILabel> lblDBName;
			NotNullPtr<UI::GUITextBox> txtDBName;
			
			static void __stdcall OnDevicesSelChg(void *userObj);
			static OSInt __stdcall ItemCompare(IO::USBInfo *item1, IO::USBInfo *item2);
		public:
			AVIRUSBDeviceForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRUSBDeviceForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
