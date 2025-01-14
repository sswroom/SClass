#ifndef _SM_SSWR_AVIREAD_AVIRNETWORKBANDWIDTHFORM
#define _SM_SSWR_AVIREAD_AVIRNETWORKBANDWIDTHFORM
#include "Data/ArrayListInt64.h"
#include "IO/Stream.h"
#include "Net/SocketMonitor.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRNetworkBandwidthForm : public UI::GUIForm
		{
		private:
			struct TimeStat
			{
				Int64 time;
				UOSInt recvCnt;
				UInt64 recvBytes;
				UOSInt sendCnt;
				UInt64 sendBytes;
			};

			struct TargetStat
			{
				Int64 addr;
				Bool updated;
				TimeStat currStat;
				TimeStat lastStat;
				NN<IO::Stream> fs;
				NN<IO::Stream> stm;
			};
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			Optional<Net::SocketMonitor> socMon;
			Sync::Mutex dispMut;
			Data::ArrayListUInt32 ipList;
			Data::UInt32FastMapNN<TargetStat> ipStats;
			Bool ipUpdated;
			Data::ArrayListInt64 macList;
			Bool macUpdated;
			Data::Int64FastMapNN<TargetStat> macStats;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblIP;
			NN<UI::GUIComboBox> cboIP;
			NN<UI::GUIButton> btnIP;
			NN<UI::GUILabel> lblMAC;
			NN<UI::GUIComboBox> cboMAC;
			NN<UI::GUIButton> btnMAC;
			NN<UI::GUIListView> lvDetail;

			static void __stdcall OnIPClicked(AnyType userObj);
			static void __stdcall OnMACClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnDataPacket(AnyType userData, UnsafeArray<const UInt8> packetData, UOSInt packetSize);
			static void __stdcall TargetStatTime(NN<TargetStat> stat, Int64 time);
			static void __stdcall TargetStatFree(NN<TargetStat> stat);
			static void __stdcall TimeStatClear(NN<TimeStat> stat);
		public:
			AVIRNetworkBandwidthForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRNetworkBandwidthForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
