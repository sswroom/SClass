#ifndef _SM_SSWR_AVIREAD_AVIRPCIDEVICEFORM
#define _SM_SSWR_AVIREAD_AVIRPCIDEVICEFORM
#include "IO/PCIInfo.h"
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
		class AVIRPCIDeviceForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListNN<IO::PCIInfo> pciList;

			NN<UI::GUIListBox> lbDevices;
			NN<UI::GUIHSplitter> hspDevices;
			NN<UI::GUIPanel> pnlDevices;
			NN<UI::GUILabel> lblVendorId;
			NN<UI::GUITextBox> txtVendorId;
			NN<UI::GUILabel> lblVendorName;
			NN<UI::GUITextBox> txtVendorName;
			NN<UI::GUILabel> lblProductId;
			NN<UI::GUITextBox> txtProductId;
			NN<UI::GUILabel> lblDispName;
			NN<UI::GUITextBox> txtDispName;
			NN<UI::GUILabel> lblDBName;
			NN<UI::GUITextBox> txtDBName;
			
			static void __stdcall OnDevicesSelChg(AnyType userObj);
			static OSInt __stdcall ItemCompare(NN<IO::PCIInfo> item1, NN<IO::PCIInfo> item2);
		public:
			AVIRPCIDeviceForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPCIDeviceForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
