#ifndef _SM_NET_TCPCLIENTMGR
#define _SM_NET_TCPCLIENTMGR
#include "AnyType.h"
#include "Data/ArrayList.h"
#include "Data/ByteArray.h"
#include "Data/FastMapNN.h"
#include "Data/SyncCircularBuffNN.h"
#include "Data/Timestamp.h"
#include "IO/SMTCWriter.h"
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
			NN<TCPClient> cli;
			AnyType cliData;
			Data::Timestamp lastDataTime;
			Sync::Mutex readMut;
			Bool reading;
			Bool processing;
			Bool timeAlerted;
			Data::Timestamp timeStart;
			Bool recvDataExist;
			UInt8 buff[TCP_BUFF_SIZE];
			UOSInt buffSize;
			void *readReq;
		};

		enum class WorkerState
		{
			NotStarted,
			Idle,
			Processing,
			Stopped
		};
		typedef struct
		{
			UOSInt index;
			WorkerState state;
			Bool toStop;
			Bool isPrimary;
			Sync::Event *evt;
			TCPClientMgr *me;
		} WorkerStatus;

		typedef void (CALLBACKFUNC TCPClientEvent)(NN<TCPClient> cli, AnyType userObj, AnyType cliData, TCPEventType evtType);
		typedef void (CALLBACKFUNC TCPClientData)(NN<TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
		typedef void (CALLBACKFUNC TCPClientTimeout)(NN<TCPClient> cli, AnyType userObj, AnyType cliData);

	private:
		TCPClientEvent evtHdlr;
		TCPClientData dataHdlr;
		TCPClientTimeout toHdlr;
		AnyType userObj;
		Data::Duration timeout;

		ClassData *clsData;
		Bool toStop;
		Bool clientThreadRunning;

		Data::UInt64FastMapNN<TCPClientStatus> cliMap;
		Sync::Mutex cliMut;

		WorkerStatus *workers;
		UOSInt workerCnt;
		Data::SyncCircularBuffNN<TCPClientStatus> workerTasks;

		IO::SMTCWriter *logWriter;

		static UInt32 __stdcall ClientThread(AnyType o);
		static UInt32 __stdcall WorkerThread(AnyType o);
		void ProcessClient(NN<TCPClientStatus> cliStat);
		void ClientBeginRead(NN<TCPClientStatus> cliStat);
	public:
		TCPClientMgr(Int32 timeOutSeconds, TCPClientEvent evtHdlr, TCPClientData dataHdlr, AnyType userObj, UOSInt workerCnt, TCPClientTimeout toHdlr);
		~TCPClientMgr();

		void SetLogFile(Text::CStringNN logFile);
		void AddClient(NN<TCPClient> cli, AnyType cliData);
		Bool SendClientData(UInt64 cliId, UnsafeArray<const UInt8> buff, UOSInt buffSize);
		Bool IsError();
		void CloseAll();

		void UseGetClient(NN<Sync::MutexUsage> mutUsage);
		UOSInt GetClientCount() const;
		void ExtendTimeout(NN<Net::TCPClient> cli);
		Optional<Net::TCPClient> GetClient(UOSInt index, OutParam<AnyType> cliData);
		IO::SMTCWriter *GetLogWriter() const;
	};
}
#endif
