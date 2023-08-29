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
			UI::GUIHSplitter *hspDevices;
			UI::GUIPanel *pnlDevices;
			UI::GUILabel *lblVendorId;
			UI::GUITextBox *txtVendorId;
			UI::GUILabel *lblVendorName;
			UI::GUITextBox *txtVendorName;
			UI::GUILabel *lblProductId;
			UI::GUITextBox *txtProductId;
			UI::GUILabel *lblDevice;
			UI::GUITextBox *txtDevice;
			UI::GUILabel *lblDispName;
			UI::GUITextBox *txtDispName;
			UI::GUILabel *lblDBName;
			UI::GUITextBox *txtDBName;
			
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
