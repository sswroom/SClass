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

			UI::GUIListBox *lbDevices;
			UI::GUIHSplitter *hspDevices;
			NotNullPtr<UI::GUIPanel> pnlDevices;
			UI::GUILabel *lblVendorId;
			UI::GUITextBox *txtVendorId;
			UI::GUILabel *lblVendorName;
			UI::GUITextBox *txtVendorName;
			UI::GUILabel *lblProductId;
			UI::GUITextBox *txtProductId;
			UI::GUILabel *lblDispName;
			UI::GUITextBox *txtDispName;
			UI::GUILabel *lblDBName;
			UI::GUITextBox *txtDBName;
			
			static void __stdcall OnDevicesSelChg(void *userObj);
			static OSInt __stdcall ItemCompare(IO::PCIInfo *item1, IO::PCIInfo *item2);
		public:
			AVIRPCIDeviceForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPCIDeviceForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
