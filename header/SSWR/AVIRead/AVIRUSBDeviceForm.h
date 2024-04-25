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
			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayList<IO::USBInfo*> usbList;

			NN<UI::GUIListBox> lbDevices;
			NN<UI::GUIHSplitter> hspDevices;
			NN<UI::GUIPanel> pnlDevices;
			NN<UI::GUILabel> lblVendorId;
			NN<UI::GUITextBox> txtVendorId;
			NN<UI::GUILabel> lblVendorName;
			NN<UI::GUITextBox> txtVendorName;
			NN<UI::GUILabel> lblProductId;
			NN<UI::GUITextBox> txtProductId;
			NN<UI::GUILabel> lblDevice;
			NN<UI::GUITextBox> txtDevice;
			NN<UI::GUILabel> lblDispName;
			NN<UI::GUITextBox> txtDispName;
			NN<UI::GUILabel> lblDBName;
			NN<UI::GUITextBox> txtDBName;
			
			static void __stdcall OnDevicesSelChg(AnyType userObj);
			static OSInt __stdcall ItemCompare(IO::USBInfo *item1, IO::USBInfo *item2);
		public:
			AVIRUSBDeviceForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRUSBDeviceForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
