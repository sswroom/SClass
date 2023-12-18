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
			NotNullPtr<UI::GUIHSplitter> hspDevices;
			NotNullPtr<UI::GUIPanel> pnlDevices;
			NotNullPtr<UI::GUILabel> lblVendorId;
			UI::GUITextBox *txtVendorId;
			NotNullPtr<UI::GUILabel> lblVendorName;
			UI::GUITextBox *txtVendorName;
			NotNullPtr<UI::GUILabel> lblProductId;
			UI::GUITextBox *txtProductId;
			NotNullPtr<UI::GUILabel> lblDispName;
			UI::GUITextBox *txtDispName;
			NotNullPtr<UI::GUILabel> lblDBName;
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
