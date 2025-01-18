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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::MODBUSTCPListener> listener;
			IO::LogTool log;
			Optional<IO::MODBUSDevSim> currDev;

			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUIButton> btnListen;
			NN<UI::GUIGroupBox> grpDev;
			NN<UI::GUILabel> lblAddr;
			NN<UI::GUITextBox> txtAddr;
			NN<UI::GUIComboBox> cboDevType;
			NN<UI::GUIButton> btnDev;
			NN<UI::GUILabel> lblDelay;
			NN<UI::GUITextBox> txtDelay;
			NN<UI::GUIButton> btnDelay;
			NN<UI::GUIListBox> lbDevice;
			NN<UI::GUIListView> lvDeviceValues;

			static void __stdcall OnListenClicked(AnyType userObj);
			static void __stdcall OnDevAddClicked(AnyType userObj);
			static void __stdcall OnDelayClicked(AnyType userObj);
			static void __stdcall OnDeviceChanged(AnyType userObj);
			static void __stdcall OnDeviceValuesDblClk(AnyType userObj, UOSInt index);
			static void __stdcall OnTimerTick(AnyType userObj);
			void UpdateDevList();
			void UpdateDevValues();
		public:
			AVIRMODBUSTCPSimForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMODBUSTCPSimForm();

			virtual void OnMonitorChanged();

			static Text::CStringNN DeviceTypeGetName(DeviceType devType);
		};
	}
}
#endif
