#ifndef _SM_SSWR_AVIREAD_AVIRMODBUSTCPSIMFORM
#define _SM_SSWR_AVIREAD_AVIRMODBUSTCPSIMFORM
#include "Net/MODBUSTCPListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMODBUSTCPSimForm : public UI::GUIForm
		{
		private:
			enum DeviceType
			{
				DT_PRINT,
				DT_ED538,
				DT_ED588,
				DT_ED516,
				DT_ED527,

				DT_FIRST = DT_PRINT,
				DT_LAST = DT_ED527
			};
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::MODBUSTCPListener *listener;
			IO::LogTool log;
			IO::MODBUSDevSim *currDev;

			UI::GUIPanel *pnlCtrl;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUIButton *btnListen;
			UI::GUIGroupBox *grpDev;
			UI::GUILabel *lblAddr;
			UI::GUITextBox *txtAddr;
			UI::GUIComboBox *cboDevType;
			UI::GUIButton *btnDev;
			UI::GUIListBox *lbDevice;
			UI::GUIListView *lvDeviceValues;

			static void __stdcall OnListenClicked(void *userObj);
			static void __stdcall OnDevAddClicked(void *userObj);
			static void __stdcall OnDeviceChanged(void *userObj);
			static void __stdcall OnDeviceValuesDblClk(void *userObj, UOSInt index);
			static void __stdcall OnTimerTick(void *userObj);
			void UpdateDevList();
			void UpdateDevValues();
		public:
			AVIRMODBUSTCPSimForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRMODBUSTCPSimForm();

			virtual void OnMonitorChanged();

			static Text::CString DeviceTypeGetName(DeviceType devType);
		};
	}
}
#endif
