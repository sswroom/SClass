#ifndef _SM_SSWR_AVIREAD_AVIRNETWORKBANDWIDTHFORM
#define _SM_SSWR_AVIREAD_AVIRNETWORKBANDWIDTHFORM
#include "Data/ArrayListInt64.h"
#include "IO/Stream.h"
#include "Net/EthernetAnalyzer.h"
#include "Net/SocketMonitor.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRNetworkBandwidthForm : public UI::GUIForm
		{
		private:
			struct DNSResult
			{
				UInt32 ip;
				NN<Text::String> name;
			};
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			Optional<Net::SocketMonitor> socMon;
			Sync::Mutex dispMut;
			Data::UInt32FastMapNN<Net::EthernetAnalyzer::BandwidthStat> ipStats;
			Data::UInt32FastMapNN<DNSResult> dnsRes;
			Optional<IO::Stream> fs;
			Optional<IO::Stream> stm;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIButton> btnLog;
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIListView> lvDetail;

			static void __stdcall OnLogClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnDataPacket(AnyType userData, UnsafeArray<const UInt8> packetData, UOSInt packetSize);
			static void __stdcall DNSResultFree(NN<DNSResult> res);
			void BandwidthStatTime(NN<Net::EthernetAnalyzer::BandwidthStat> stat, Int64 time);
		public:
			AVIRNetworkBandwidthForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRNetworkBandwidthForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
