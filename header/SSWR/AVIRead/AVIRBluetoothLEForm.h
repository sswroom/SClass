#ifndef _SM_SSWR_AVIREAD_AVIRBLUETOOTHLEFORM
#define _SM_SSWR_AVIREAD_AVIRBLUETOOTHLEFORM
#include "Data/FastMap.h"
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
				UInt64 mac;
				Text::String *name;
				Int32 rssi;
				Bool updated;
				Bool shown;
			} BTDevice;
			
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::BTManager btMgr;
			Data::ArrayList<IO::BTController *> btList;
			IO::BTController *btCtrl;
			Sync::Mutex devMut;
			Data::FastMap<UInt64, BTDevice*> devMap;

			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUILabel> lblInterface;
			NotNullPtr<UI::GUIComboBox> cboInterface;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUIButton> btnStoreList;
			UI::GUIListView *lvDevices;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnStoreListClicked(void *userObj);
			static void __stdcall OnDevicesDblClick(void *userObj, UOSInt index);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnLEScanItem(void *userObj, UInt64 mac, Int32 rssi, Text::CString name);
			void ClearDevices();
		public:
			AVIRBluetoothLEForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBluetoothLEForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
