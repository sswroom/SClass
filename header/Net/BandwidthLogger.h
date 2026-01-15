#ifndef _SM_NET_BANDWIDTHLOGGER
#define _SM_NET_BANDWIDTHLOGGER
#include "IO/Stream.h"
#include "Net/EthernetAnalyzer.h"
#include "Net/SocketMonitor.h"
#include "Sync/MutexUsage.h"

namespace Net
{
	class BandwidthLogger
	{
	private:
		struct DNSResult
		{
			UInt32 ip;
			NN<Text::String> name;
		};
		NN<Net::SocketFactory> sockf;
		Optional<Net::SocketMonitor> socMon;
		Sync::Mutex mut;
		Data::UInt32FastMapNN<Net::EthernetAnalyzer::BandwidthStat> ipStats;
		Data::UInt32FastMapNN<DNSResult> dnsRes;
		Optional<IO::Stream> fs;
		Optional<IO::Stream> stm;
		Int64 logBeginTime;

		static void __stdcall OnDataPacket(AnyType userData, UnsafeArray<const UInt8> packetData, UIntOS packetSize);
		static void __stdcall DNSResultFree(NN<DNSResult> res);
		void BandwidthStatTime(NN<Net::EthernetAnalyzer::BandwidthStat> stat, Int64 time);
	public:
		BandwidthLogger(NN<Net::SocketFactory> sockf);
		~BandwidthLogger();

		Bool IsError() const { return this->socMon.IsNull(); }
		Bool BeginLogFile(Text::CStringNN fileName);
		void EndLogFile();
		Bool IsLogging() const { return this->stm.NotNull(); }
		NN<const Data::UInt32FastMapNN<Net::EthernetAnalyzer::BandwidthStat>> GetIPStats(NN<Sync::MutexUsage> mutUsage) const;

		Optional<Text::String> GetIPName(UInt32 sortIP, NN<Sync::MutexUsage> mutUsage) const;
	};
}
#endif
