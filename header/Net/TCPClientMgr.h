#ifndef _SM_NET_TCPCLIENTMGR
#define _SM_NET_TCPCLIENTMGR
#include "Data/ArrayList.h"
#include "Data/ArrayListUInt64.h"
#include "Data/DateTime.h"
#include "Data/SyncCircularBuff.h"
#include "Net/TCPClient.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

#define TCP_BUFF_SIZE 16384

namespace Net
{
	class TCPClientMgr
	{
	public:
		struct ClassData;
		typedef enum
		{
			TCP_EVENT_CONNECT,
			TCP_EVENT_DISCONNECT,
			TCP_EVENT_HASDATA,
			TCP_EVENT_SHUTDOWN
		} TCPEventType;

		class TCPClientStatus
		{
		public:
			UTF8Char debug[6];
			TCPClient *cli;
			void *cliData;
			Int64 lastDataTimeTicks;
			Sync::Mutex readMut;
			Bool reading;
			Bool processing;
			Bool timeAlerted;
			Int64 timeStart;
			Bool recvDataExist;
			UInt8 buff[TCP_BUFF_SIZE];
			UOSInt buffSize;
			void *readReq;
		};

		typedef struct
		{
			Bool running;
			Bool toStop;
			Bool isPrimary;
			Bool working;
			Sync::Event *evt;
			TCPClientMgr *me;
		} WorkerStatus;

		typedef void (__stdcall *TCPClientEvent)(TCPClient *cli, void *userObj, void *cliData, TCPEventType evtType);
		typedef void (__stdcall *TCPClientData)(TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
		typedef void (__stdcall *TCPClientTimeout)(TCPClient *cli, void *userObj, void *cliData);

	private:
		TCPClientEvent evtHdlr;
		TCPClientData dataHdlr;
		TCPClientTimeout toHdlr;
		void *userObj;
		Int32 timeOutSeconds;

		ClassData *clsData;
		Bool toStop;
		Bool clientThreadRunning;

		Data::ArrayListUInt64 cliIdArr;
		Data::ArrayList<TCPClientStatus*> cliArr;
		Sync::Mutex cliMut;

		WorkerStatus *workers;
		UOSInt workerCnt;
		Data::SyncCircularBuff<TCPClientStatus*> workerTasks;

		static UInt32 __stdcall ClientThread(void *o);
		static UInt32 __stdcall WorkerThread(void *o);
		void ProcessClient(TCPClientStatus *cliStat);
	public:
		TCPClientMgr(Int32 timeOutSeconds, TCPClientEvent evtHdlr, TCPClientData dataHdlr, void *userObj, UOSInt workerCnt, TCPClientTimeout toHdlr);
		~TCPClientMgr();

		void AddClient(TCPClient *cli, void *cliData);
		Bool SendClientData(UInt64 cliId, const UInt8 *buff, UOSInt buffSize);
		Bool IsError();
		void CloseAll();

		void UseGetClient(Sync::MutexUsage *mutUsage);
		UOSInt GetClientCount();
		void ExtendTimeout(Net::TCPClient *cli);
		Net::TCPClient *GetClient(UOSInt index, void **cliData);
	};
}
#endif
