#ifndef _SM_NET_NETATTACKDETECTOR
#define _SM_NET_NETATTACKDETECTOR
#include "AnyType.h"
#include "Data/FastMapNative.hpp"
#include "Data/FastMapNN.hpp"
#include "Net/SocketFactory.h"
#include "Net/SocketMonitor.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Net
{
	class NetAttackDetector
	{
	public:
		enum class AttackType
		{
			IPScan,
			TCPPortScan,
			CVE_2026_98432
		};

		typedef void (CALLBACKFUNC AttackHandler)(NN<Net::SocketUtil::AddressInfo> addr, AttackType attackType, AnyType userData);

		struct AdapterStatus
		{
			UInt64 iMAC;
			Int64 lastDetectTime;
			IntOS detectCnt;
			Data::FastMapNative<UInt32, Int64> targetIPMap;
		};

		struct TCPSourceStatus
		{
			Int64 lastDetectTime;
			IntOS detectCnt;
			Int64 lastCVEAlertTime;
			Data::FastMapNative<UInt16, Int64> targetPortMap;
		};

		struct TCPSourceIPv6Key
		{
			UInt64 high;
			UInt64 low;

			TCPSourceIPv6Key(UInt64 high = 0, UInt64 low = 0)
			{
				this->high = high;
				this->low = low;
			}

			Bool operator==(const TCPSourceIPv6Key &other) const
			{
				return this->high == other.high && this->low == other.low;
			}

			Bool operator>(const TCPSourceIPv6Key &other) const
			{
				return this->high > other.high || (this->high == other.high && this->low > other.low);
			}

			Bool operator<(const TCPSourceIPv6Key &other) const
			{
				return this->high < other.high || (this->high == other.high && this->low < other.low);
			}
		};

	private:
		NN<Net::SocketFactory> sockf;
		Optional<SocketMonitor> socMon;
		AttackHandler hdlr;
		AnyType userData;
		Data::FastMapNN<UInt64, AdapterStatus> adapterMap;
		Data::FastMapNN<UInt32, TCPSourceStatus> tcpSourceMap;
		Data::FastMapNN<TCPSourceIPv6Key, TCPSourceStatus> tcpSourceIPv6Map;
		Sync::Mutex adapterMut;

	private:
		static void __stdcall OnRAWData(AnyType userData, UnsafeArray<const UInt8> packetData, UIntOS packetSize);

	public:
		NetAttackDetector(NN<Net::SocketFactory> sockf, AttackHandler hdlr, AnyType userData, UIntOS workerCnt);
		~NetAttackDetector();

		Bool IsError();
	};
}
#endif
