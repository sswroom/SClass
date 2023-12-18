#ifndef _SM_SSWR_AVIREAD_AVIRBLUETOOTHCTLFORM
#define _SM_SSWR_AVIREAD_AVIRBLUETOOTHCTLFORM
#include "Data/FastMap.h"
#include "IO/BTScanner.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBluetoothCtlForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Sync::Mutex devMut;
			Data::FastMap<UInt64, UInt32> randDevMap;
			Data::FastMap<UInt64, UInt32> pubDevMap;
			IO::BTScanner *bt;

			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUIButton> btnStoreList;
			UI::GUIListView *lvDevices;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnStoreListClicked(void *userObj);
			static void __stdcall OnDevicesDblClick(void *userObj, UOSInt index);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnDeviceUpdated(IO::BTScanLog::ScanRecord3 *dev, IO::BTScanner::UpdateType updateType, void *userObj);
			UOSInt UpdateList(NotNullPtr<Data::FastMap<UInt64, IO::BTScanLog::ScanRecord3*>> devMap, Data::FastMap<UInt64, UInt32> *statusMap, UOSInt baseIndex);
		public:
			AVIRBluetoothCtlForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBluetoothCtlForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
