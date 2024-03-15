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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayList<IO::PCIInfo*> pciList;

			NotNullPtr<UI::GUIListBox> lbDevices;
			NotNullPtr<UI::GUIHSplitter> hspDevices;
			NotNullPtr<UI::GUIPanel> pnlDevices;
			NotNullPtr<UI::GUILabel> lblVendorId;
			NotNullPtr<UI::GUITextBox> txtVendorId;
			NotNullPtr<UI::GUILabel> lblVendorName;
			NotNullPtr<UI::GUITextBox> txtVendorName;
			NotNullPtr<UI::GUILabel> lblProductId;
			NotNullPtr<UI::GUITextBox> txtProductId;
			NotNullPtr<UI::GUILabel> lblDispName;
			NotNullPtr<UI::GUITextBox> txtDispName;
			NotNullPtr<UI::GUILabel> lblDBName;
			NotNullPtr<UI::GUITextBox> txtDBName;
			
			static void __stdcall OnDevicesSelChg(AnyType userObj);
			static OSInt __stdcall ItemCompare(IO::PCIInfo *item1, IO::PCIInfo *item2);
		public:
			AVIRPCIDeviceForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPCIDeviceForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
