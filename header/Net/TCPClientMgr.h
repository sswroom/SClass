#ifndef _SM_NET_TCPCLIENTMGR
#define _SM_NET_TCPCLIENTMGR
#include "Data/DateTime.h"
#include "Sync/Mutex.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt64.h"
#include "Data/LinkedList.h"
#include "Net/TCPClient.h"
#include "Sync/MutexUsage.h"

namespace Net
{
	class TCPClientMgr
	{
	public:
		typedef enum
		{
			TCP_EVENT_CONNECT,
			TCP_EVENT_DISCONNECT,
			TCP_EVENT_HASDATA,
			TCP_EVENT_SHUTDOWN
		} TCPEventType;

		typedef struct
		{
			TCPClient *cli;
			void *cliData;
			Data::DateTime *lastDataTime;
			Sync::Mutex *readMut;
			Bool reading;
			Bool processing;
			Bool timeAlerted;
			Int64 timeStart;
			Bool recvDataExist;
			UInt8 *buff;
			UOSInt buffSize;
			void *readReq;
		} TCPClientStatus;

		typedef struct
		{
			Bool running;
			Bool toStop;
			Sync::Event *evt;
			TCPClientStatus *cliStat;
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

		void *clsData;
		Bool toStop;
		Bool clientThreadRunning;

		Data::ArrayListInt64 *cliIdArr;
		Data::ArrayList<TCPClientStatus*> *cliArr;
		Sync::Mutex *cliMut;

		WorkerStatus *workers;
		UOSInt workerCnt;
		Data::LinkedList *workerTasks;

		static UInt32 __stdcall ClientThread(void *o);
		static UInt32 __stdcall WorkerThread(void *o);
		void ProcessClient(TCPClientStatus *cliStat);
	public:
		TCPClientMgr(Int32 timeOutSeconds, TCPClientEvent evtHdlr, TCPClientData dataHdlr, void *userObj, UOSInt workerCnt, TCPClientTimeout toHdlr);
		~TCPClientMgr();

		void AddClient(TCPClient *cli, void *cliData);
		Bool SendClientData(Int64 cliId, const UInt8 *buff, UOSInt buffSize);
		Bool IsError();
		void CloseAll();

		void UseGetClient(Sync::MutexUsage *mutUsage);
		OSInt GetClientCount();
		void ExtendTimeout(Net::TCPClient *cli);
		Net::TCPClient *GetClient(OSInt index, void **cliData);
	};
}
#endif
