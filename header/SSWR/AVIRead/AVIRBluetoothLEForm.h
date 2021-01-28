#ifndef _SM_SSWR_AVIREAD_AVIRBLUETOOTHLEFORM
#define _SM_SSWR_AVIREAD_AVIRBLUETOOTHLEFORM
#include "Data/Integer64Map.h"
#include "IO/BTManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBluetoothLEForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				Int64 mac;
				const UTF8Char *name;
				Int32 rssi;
				Bool updated;
				Bool shown;
			} BTDevice;
			
		private:
			SSWR::AVIRead::AVIRCore *core;
			IO::BTManager *btMgr;
			Data::ArrayList<IO::BTController *> *btList;
			IO::BTController *btCtrl;
			Sync::Mutex *devMut;
			Data::Integer64Map<BTDevice*> *devMap;

			UI::GUIPanel *pnlControl;
			UI::GUILabel *lblInterface;
			UI::GUIComboBox *cboInterface;
			UI::GUIButton *btnStart;
			UI::GUIButton *btnStoreList;
			UI::GUIListView *lvDevices;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnStoreListClicked(void *userObj);
			static void __stdcall OnDevicesDblClick(void *userObj, OSInt index);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnLEScanItem(void *userObj, Int64 mac, Int32 rssi, const Char *name);
			void ClearDevices();
		public:
			AVIRBluetoothLEForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRBluetoothLEForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
