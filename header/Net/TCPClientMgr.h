#ifndef _SM_NET_TCPCLIENTMGR
#define _SM_NET_TCPCLIENTMGR
#include "AnyType.h"
#include "Data/ArrayList.h"
#include "Data/ByteArray.h"
#include "Data/FastMap.h"
#include "Data/SyncCircularBuff.h"
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
			NotNullPtr<TCPClient> cli;
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

		typedef void (__stdcall *TCPClientEvent)(NotNullPtr<TCPClient> cli, AnyType userObj, AnyType cliData, TCPEventType evtType);
		typedef void (__stdcall *TCPClientData)(NotNullPtr<TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
		typedef void (__stdcall *TCPClientTimeout)(NotNullPtr<TCPClient> cli, AnyType userObj, AnyType cliData);

	private:
		TCPClientEvent evtHdlr;
		TCPClientData dataHdlr;
		TCPClientTimeout toHdlr;
		AnyType userObj;
		Data::Duration timeout;

		ClassData *clsData;
		Bool toStop;
		Bool clientThreadRunning;

		Data::UInt64FastMap<TCPClientStatus*> cliMap;
		Sync::Mutex cliMut;

		WorkerStatus *workers;
		UOSInt workerCnt;
		Data::SyncCircularBuff<TCPClientStatus*> workerTasks;

		IO::SMTCWriter *logWriter;

		static UInt32 __stdcall ClientThread(void *o);
		static UInt32 __stdcall WorkerThread(void *o);
		void ProcessClient(NotNullPtr<TCPClientStatus> cliStat);
		void ClientBeginRead(NotNullPtr<TCPClientStatus> cliStat);
	public:
		TCPClientMgr(Int32 timeOutSeconds, TCPClientEvent evtHdlr, TCPClientData dataHdlr, AnyType userObj, UOSInt workerCnt, TCPClientTimeout toHdlr);
		~TCPClientMgr();

		void SetLogFile(Text::CStringNN logFile);
		void AddClient(NotNullPtr<TCPClient> cli, AnyType cliData);
		Bool SendClientData(UInt64 cliId, const UInt8 *buff, UOSInt buffSize);
		Bool IsError();
		void CloseAll();

		void UseGetClient(NotNullPtr<Sync::MutexUsage> mutUsage);
		UOSInt GetClientCount() const;
		void ExtendTimeout(NotNullPtr<Net::TCPClient> cli);
		Net::TCPClient *GetClient(UOSInt index, OutParam<AnyType> cliData);
		IO::SMTCWriter *GetLogWriter() const;
	};
}
#endif
