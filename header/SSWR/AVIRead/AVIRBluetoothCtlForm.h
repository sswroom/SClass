#ifndef _SM_SSWR_AVIREAD_AVIRBLUETOOTHCTLFORM
#define _SM_SSWR_AVIREAD_AVIRBLUETOOTHCTLFORM
#include "Data/UInt64Map.h"
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
			SSWR::AVIRead::AVIRCore *core;
			Sync::Mutex devMut;
			Data::UInt64Map<UInt32> randDevMap;
			Data::UInt64Map<UInt32> pubDevMap;
			IO::BTScanner *bt;

			UI::GUIPanel *pnlControl;
			UI::GUIButton *btnStart;
			UI::GUIButton *btnStoreList;
			UI::GUIListView *lvDevices;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnStoreListClicked(void *userObj);
			static void __stdcall OnDevicesDblClick(void *userObj, UOSInt index);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnDeviceUpdated(IO::BTScanLog::ScanRecord3 *dev, IO::BTScanner::UpdateType updateType, void *userObj);
			UOSInt UpdateList(Data::UInt64Map<IO::BTScanLog::ScanRecord3*> *devMap, Data::UInt64Map<UInt32> *statusMap, UOSInt baseIndex);
		public:
			AVIRBluetoothCtlForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRBluetoothCtlForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
