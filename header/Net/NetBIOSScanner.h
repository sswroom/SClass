#ifndef _SM_NET_NETBIOSSCANNER
#define _SM_NET_NETBIOSSCANNER
#include "Data/CallbackStorage.h"
#include "Data/FastMap.h"
#include "Net/UDPServer.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace Net
{
	class NetBIOSScanner
	{
	public:
		struct NameEntry
		{
			UTF8Char nameBuff[16];
			UInt8 nameType;
			UInt16 flags;
		};

		struct NameAnswer
		{
			UInt32 sortableIP;
			UInt32 ttl;
			UInt8 unitId[6];
			NameEntry *names;
			UOSInt namesCnt;
		};

		typedef void (__stdcall *AnswerUpdated)(AnyType userObj, UInt32 sortableIP);
	private:
		Net::UDPServer *svr;
		Sync::Mutex ansMut;
		Data::FastMap<UInt32, NameAnswer*> answers;
		Data::CallbackStorage<AnswerUpdated> hdlr;

		static void __stdcall OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
		static void FreeAnswer(NameAnswer *ans);
	public:
		NetBIOSScanner(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log);
		~NetBIOSScanner();

		Bool IsError() const;
		void SendRequest(UInt32 ip);
		void SetAnswerHandler(AnswerUpdated hdlr, AnyType userObj);
		NN<const Data::ReadingList<NameAnswer*>> GetAnswers(NN<Sync::MutexUsage> mutUsage) const;
	};
}
#endif
