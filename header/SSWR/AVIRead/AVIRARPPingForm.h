#ifndef _SM_SSWR_AVIREAD_AVIRARPPINGFORM
#define _SM_SSWR_AVIREAD_AVIRARPPINGFORM

#include "IO/LogTool.h"
#include "Manage/HiResClock.h"
#include "Net/ARPHandler.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Event.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRealtimeLineChart.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRARPPingForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				NN<Text::String> ifName;
				UInt32 ipAddr;
				UInt8 hwAddr[6];
			} AdapterInfo;
			
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			Net::ARPHandler *arpHdlr;
			Net::SocketUtil::AddressInfo targetAddr;
			Manage::HiResClock clk;
			Bool requested;
			Sync::Event *reqEvt;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;
			Data::ArrayListNN<AdapterInfo> adapters;

			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUIVSplitter> vspRequest;
			NN<UI::GUILabel> lblAdapter;
			NN<UI::GUIComboBox> cboAdapter;
			NN<UI::GUILabel> lblTarget;
			NN<UI::GUITextBox> txtTarget;
			NN<UI::GUILabel> lblRepeat;
			NN<UI::GUICheckBox> chkRepeat;
			NN<UI::GUIButton> btnPing;
			NN<UI::GUIRealtimeLineChart> rlcPing;
			NN<UI::GUIVSplitter> vspPing;
			NN<UI::GUIListBox> lbLog;

			static void __stdcall OnARPHandler(const UInt8 *hwAddr, UInt32 ipAddr, AnyType userObj);
			static void __stdcall OnPingClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRARPPingForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRARPPingForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
