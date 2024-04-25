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
			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayList<IO::HIDInfo*> hidList;

			NN<UI::GUIListBox> lbDevices;
			NN<UI::GUIHSplitter> hspDevices;
			NN<UI::GUIPanel> pnlDevices;
			NN<UI::GUILabel> lblVendorId;
			NN<UI::GUITextBox> txtVendorId;
			NN<UI::GUILabel> lblProductId;
			NN<UI::GUITextBox> txtProductId;
			NN<UI::GUILabel> lblDevPath;
			NN<UI::GUITextBox> txtDevPath;
			
			static void __stdcall OnDevicesSelChg(AnyType userObj);
			static OSInt __stdcall ItemCompare(IO::HIDInfo *item1, IO::HIDInfo *item2);
		public:
			AVIRHIDDeviceForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHIDDeviceForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
