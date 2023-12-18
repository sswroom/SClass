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
			enum class DeviceType
			{
				Print,
				ED538,
				ED588,
				ED516,
				ED527,

				First = Print,
				Last = ED527
			};
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::MODBUSTCPListener *listener;
			IO::LogTool log;
			IO::MODBUSDevSim *currDev;

			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUILabel> lblPort;
			UI::GUITextBox *txtPort;
			NotNullPtr<UI::GUIButton> btnListen;
			NotNullPtr<UI::GUIGroupBox> grpDev;
			NotNullPtr<UI::GUILabel> lblAddr;
			UI::GUITextBox *txtAddr;
			NotNullPtr<UI::GUIComboBox> cboDevType;
			NotNullPtr<UI::GUIButton> btnDev;
			NotNullPtr<UI::GUILabel> lblDelay;
			UI::GUITextBox *txtDelay;
			NotNullPtr<UI::GUIButton> btnDelay;
			UI::GUIListBox *lbDevice;
			UI::GUIListView *lvDeviceValues;

			static void __stdcall OnListenClicked(void *userObj);
			static void __stdcall OnDevAddClicked(void *userObj);
			static void __stdcall OnDelayClicked(void *userObj);
			static void __stdcall OnDeviceChanged(void *userObj);
			static void __stdcall OnDeviceValuesDblClk(void *userObj, UOSInt index);
			static void __stdcall OnTimerTick(void *userObj);
			void UpdateDevList();
			void UpdateDevValues();
		public:
			AVIRMODBUSTCPSimForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMODBUSTCPSimForm();

			virtual void OnMonitorChanged();

			static Text::CStringNN DeviceTypeGetName(DeviceType devType);
		};
	}
}
#endif
