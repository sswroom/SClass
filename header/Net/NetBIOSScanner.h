#ifndef _SM_NET_NETBIOSSCANNER
#define _SM_NET_NETBIOSSCANNER
#include "Data/UInt32Map.h"
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

		typedef void (__stdcall *AnswerUpdated)(void *userObj, UInt32 sortableIP);
	private:
		Net::UDPServer *svr;
		Sync::Mutex ansMut;
		Data::UInt32Map<NameAnswer*> answers;
		AnswerUpdated hdlr;
		void *hdlrObj;

		static void __stdcall OnUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
		static void FreeAnswer(NameAnswer *ans);
	public:
		NetBIOSScanner(Net::SocketFactory *sockf);
		~NetBIOSScanner();

		Bool IsError() const;
		void SendRequest(UInt32 ip);
		void SetAnswerHandler(AnswerUpdated hdlr, void *userObj);
		const Data::ArrayList<NameAnswer*> *GetAnswers(Sync::MutexUsage *mutUsage) const;
	};
}
#endif
