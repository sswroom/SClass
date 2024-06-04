#ifndef _SM_SSWR_SMONITOR_SMONITORSVRCORE
#define _SM_SSWR_SMONITOR_SMONITORSVRCORE
#include "Crypto/Hash/CRC32RC.h"
#include "Crypto/Hash/HashCalc.h"
#include "Data/FastMap.h"
#include "Data/StringUTF8Map.h"
#include "DB/DBTool.h"
#include "IO/Writer.h"
#include "IO/StringLogger.h"
#include "IO/ProtoHdlr/ProtoSMonHandler.h"
#include "Net/TCPClientMgr.h"
#include "Net/TCPServer.h"
#include "Net/UDPServer.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"
#include "Net/WebServer/WebListener.h"
#include "Parser/ParserList.h"
#include "SSWR/SMonitor/ISMonitorCore.h"
#include "Sync/Mutex.h"
#include "Sync/RWMutex.h"
#include "Sync/Thread.h"

namespace SSWR
{
	namespace SMonitor
	{
		class SMonitorSvrCore : public IO::IProtocolHandler::DataListener, public SSWR::SMonitor::ISMonitorCore, public Net::WebServer::IReqLogger
		{
		private:
			typedef struct
			{
				UInt8 *dataBuff;
				UOSInt dataSize;
				Int64 cliId;
				NN<SMonitorSvrCore> me;
				AnyType stmData;
				Optional<DeviceInfo> dev;
			} ClientStatus;

		private:
			NN<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			IO::LogTool log;
			Net::TCPServer *cliSvr;
			Net::TCPClientMgr *cliMgr;
			IO::ProtoHdlr::ProtoSMonHandler protoHdlr;
			Net::UDPServer *notifyUDP;
			Net::UDPServer *dataUDP;
			Crypto::Hash::HashCalc *dataCRC;
			Sync::Mutex notifyCRCMut;
			Crypto::Hash::CRC32RC notifyCRC;
			Text::String *notifyPwd;
			Net::WebServer::WebListener *listener;
			Net::WebServer::HTTPDirectoryHandler *webHdlr;
			Text::String *dataDir;
			NN<Media::DrawEngine> deng;
			NN<Parser::ParserList> parsers;
			Bool initErr;

			Optional<DB::DBTool> db;
			Sync::Mutex *dbMut;
			Sync::Thread thread;
			Int64 currDate;
			Sync::RWMutex dateMut;

			Sync::RWMutex devMut;
			Data::FastMapNN<Int64, DeviceInfo> devMap;
			Sync::RWMutex userMut;
			Data::FastMapNN<Int32, WebUser> userMap;
			Data::StringUTF8Map<WebUser*> userNameMap;

			IO::StringLogger uaLog;
			IO::StringLogger refererLog;

			static void __stdcall OnClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall OnClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
			static void __stdcall OnClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
			static void __stdcall OnServerConn(NN<Socket> s, AnyType userObj);
			static void __stdcall CheckThread(NN<Sync::Thread> thread);
			static void __stdcall OnDataUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
			static void __stdcall OnNotifyUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);

			virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
			virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, const UInt8 *buff, UOSInt buffSize);

			void NewNotify(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Timestamp ts, UInt8 type, UInt32 procId, Text::CStringNN progName);

			void TCPSendLoginReply(NN<IO::Stream> stm, Int64 cliTime, Int64 svrTime, UInt8 status);
			void TCPSendKAReply(NN<IO::Stream> stm, Int64 cliTime, Int64 svrTime);
			void TCPSendCapturePhoto(NN<IO::Stream> stm);
			void TCPSendPhotoEnd(NN<IO::Stream> stm, Int64 photoTime);
			void TCPSendSetOutput(NN<IO::Stream> stm, UInt32 outputNum, Bool toHigh);

			void UDPSendReadingRecv(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Int64 recTime);
			void UDPSendCapturePhoto(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port);
			void UDPSendPhotoPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Int64 photoTime, UInt32 seq);
			void UDPSendPhotoEnd(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Int64 photoTime);
			void UDPSendSetOutput(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, UInt8 outputNum, Bool isHigh);

			void SaveDatas();
			void SavePhoto(Int64 cliId, Int64 photoTime, Int32 photoFmt, UInt8 *photoBuff, UOSInt photoSize);

			void LoadData();
			Optional<DB::DBTool> UseDB(NN<Sync::MutexUsage> mut);
			void UserPwdCalc(UnsafeArray<const UTF8Char> userName, UnsafeArray<const UTF8Char> pwd, UInt8 *buff);
		public:
			SMonitorSvrCore(NN<IO::Writer> writer, NN<Media::DrawEngine> deng);
			virtual ~SMonitorSvrCore();

			Bool IsError();
			virtual NN<Media::DrawEngine> GetDrawEngine();

			Optional<DeviceInfo> DevGet(Int64 cliId);
			Optional<DeviceInfo> DevGetOrAdd(Int64 cliId);
			Optional<DeviceInfo> DevAdd(Int64 cliId, Text::CStringNN cpuName, Text::CStringNN platformName);
			Bool DeviceRecvReading(NN<DeviceInfo> dev, Int64 cliTime, UOSInt nDigitals, UOSInt nReading, UOSInt nOutput, UInt32 digitalVals, ReadingInfo *readings, Int32 profileId, UInt32 cliIP, UInt16 port);
			Bool DeviceKARecv(NN<DeviceInfo> dev, Int64 kaTime);
			Bool DeviceSetName(Int64 cliId, NN<Text::String> devName);
			Bool DeviceSetPlatform(Int64 cliId, NN<Text::String> platformName);
			Bool DeviceSetCPUName(Int64 cliId, NN<Text::String> cpuName);
			Bool DeviceSetReading(Int64 cliId, UInt32 index, UInt16 sensorId, UInt16 readingId, UnsafeArray<const UTF8Char> readingName);
			Bool DeviceSetVersion(Int64 cliId, Int64 version);
			virtual Optional<DeviceInfo> DeviceGet(Int64 cliId);
			virtual Bool DeviceModify(Int64 cliId, Text::CString devName, Int32 flags);
			virtual Bool DeviceSetReadings(NN<DeviceInfo> dev, UnsafeArrayOpt<const UTF8Char> readings);
			virtual Bool DeviceSetDigitals(NN<DeviceInfo> dev, UnsafeArrayOpt<const UTF8Char> digitals);
			virtual UOSInt DeviceQueryRec(Int64 cliId, Int64 startTime, Int64 endTime, NN<Data::ArrayListNN<DevRecord2>> recList);
			virtual Bool DeviceSetOutput(Int64 cliId, UInt32 outputNum, Bool toHigh);

			virtual Bool UserExist();
			virtual Bool UserAdd(UnsafeArray<const UTF8Char> userName, UnsafeArray<const UTF8Char> password, Int32 userType);
			virtual Bool UserSetPassword(Int32 userId, UnsafeArray<const UTF8Char> password);
			virtual Optional<LoginInfo> UserLogin(UnsafeArray<const UTF8Char> userName, UnsafeArray<const UTF8Char> password);
			virtual void UserFreeLogin(NN<LoginInfo> login);
			virtual UOSInt UserGetDevices(Int32 userId, Int32 userType, NN<Data::ArrayListNN<DeviceInfo>> devList);
			virtual Bool UserHasDevice(Int32 userId, Int32 userType, Int64 cliId);
			virtual UOSInt UserGetList(NN<Data::ArrayListNN<WebUser>> userList);
			virtual Optional<WebUser> UserGet(Int32 userId);
			virtual Bool UserAssign(Int32 userId, NN<Data::ArrayList<Int64>> devIdList);

			virtual Bool SendCapturePhoto(Int64 cliId);

			virtual void LogRequest(NN<Net::WebServer::IWebRequest> req);

			void UserAgentLog(UnsafeArray<const UTF8Char> userAgent, UOSInt len);
			void UserAgentStore();
			
			void RefererLog(UnsafeArray<const UTF8Char> referer, UOSInt len);
			void RefererStore();
		};
	}
}

#endif
