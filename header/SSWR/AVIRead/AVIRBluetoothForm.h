#ifndef _SM_SSWR_AVIREAD_AVIRBLUETOOTHFORM
#define _SM_SSWR_AVIREAD_AVIRBLUETOOTHFORM
#include "IO/BTManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBluetoothForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				IO::BTController *bt;
				Data::ArrayList<IO::BTController::BTDevice*> *devList;
			} BTStatus;
		private:
			UI::GUIListBox *lbCtrl;
			UI::GUIHSplitter *hspMain;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpInfo;
			UI::GUILabel *lblRadioName;
			UI::GUITextBox *txtRadioName;
			UI::GUILabel *lblAddr;
			UI::GUITextBox *txtAddr;
			UI::GUILabel *lblManu;
			UI::GUITextBox *txtManu;
			UI::GUILabel *lblSubver;
			UI::GUITextBox *txtSubver;
			UI::GUILabel *lblClass;
			UI::GUITextBox *txtClass;

			UI::GUITabPage *tpDevice;
			UI::GUIPanel *pnlDevice;
			UI::GUIButton *btnDeviceSrch;
			UI::GUIButton *btnDeviceUpdate;
			UI::GUIPanel *pnlDevDetail;
			UI::GUIVSplitter *vspDevice;
			UI::GUIPanel *pnlDevCtrl;
			UI::GUIButton *btnDevAuthen;
			UI::GUIButton *btnDevUnauthen;
			UI::GUIListBox *lbDevServices;
			UI::GUIListView *lvDevice;

			Data::ArrayList<BTStatus*> btList;
			IO::BTController::BTDevice *currDev;
			Data::ArrayList<void *> guidList;

			SSWR::AVIRead::AVIRCore *core;

			static void __stdcall OnCtrlChanged(void *userObj);
			static void __stdcall OnDeviceSrchClicked(void *userObj);
			static void __stdcall OnDeviceUpdateClicked(void *userObj);
			static void __stdcall OnDeviceSelChg(void *userObj);
			static void __stdcall OnDevAuthenClicked(void *userObj);
			static void __stdcall OnDevUnauthenClicked(void *userObj);

			void ClearGUIDs();
			void UpdateDevList(BTStatus *btStatus);
		public:
			AVIRBluetoothForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRBluetoothForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
