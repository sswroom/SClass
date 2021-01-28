#ifndef _SM_SSWR_SMONITOR_SMONITORSVRCORE
#define _SM_SSWR_SMONITOR_SMONITORSVRCORE
#include "Crypto/Hash/CRC16.h"
#include "Data/Integer32Map.h"
#include "Data/Integer64Map.h"
#include "Data/StringUTF8Map.h"
#include "DB/DBTool.h"
#include "IO/IWriter.h"
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
				SMonitorSvrCore *me;
				void *stmData;
				DeviceInfo *dev;
			} ClientStatus;

		private:
			Net::SocketFactory *sockf;
			IO::LogTool *log;
			Net::TCPServer *cliSvr;
			Net::TCPClientMgr *cliMgr;
			IO::ProtoHdlr::ProtoSMonHandler *protoHdlr;
			Net::UDPServer *dataUDP;
			Sync::Mutex *dataCRCMut;
			Crypto::Hash::CRC16 *dataCRC;
			Net::WebServer::WebListener *listener;
			Net::WebServer::HTTPDirectoryHandler *webHdlr;
			const UTF8Char *dataDir;
			Media::DrawEngine *deng;
			Parser::ParserList *parsers;
			Bool initErr;

			DB::DBTool *db;
			Sync::Mutex *dbMut;
			Bool checkRunning;
			Bool checkToStop;
			Sync::Event *checkEvt;
			Int64 currDate;
			Sync::RWMutex *dateMut;

			Sync::RWMutex *devMut;
			Data::Integer64Map<DeviceInfo*> *devMap;
			Sync::RWMutex *userMut;
			Data::Integer32Map<WebUser*> *userMap;
			Data::StringUTF8Map<WebUser*> *userNameMap;

			Sync::Mutex *uaMut;
			Data::ArrayListStrUTF8 *uaList;
			Bool uaModified;
			Sync::Mutex *refererMut;
			Data::ArrayListStrUTF8 *refererList;
			Bool refererModified;

			static void __stdcall OnClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall OnClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
			static void __stdcall OnClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData);
			static void __stdcall OnServerConn(UInt32 *s, void *userObj);
			static UInt32 __stdcall CheckThread(void *userObj);
			static void __stdcall OnDataUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);

			virtual void DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
			virtual void DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize);

			void TCPSendLoginReply(IO::Stream *stm, Int64 cliTime, Int64 svrTime, UInt8 status);
			void TCPSendKAReply(IO::Stream *stm, Int64 cliTime, Int64 svrTime);
			void TCPSendCapturePhoto(IO::Stream *stm);
			void TCPSendPhotoEnd(IO::Stream *stm, Int64 photoTime);
			void TCPSendSetOutput(IO::Stream *stm, Int32 outputNum, Bool toHigh);

			void UDPSendReadingRecv(const Net::SocketUtil::AddressInfo *addr, UInt16 port, Int64 recTime);
			void UDPSendCapturePhoto(const Net::SocketUtil::AddressInfo *addr, UInt16 port);
			void UDPSendPhotoPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, Int64 photoTime, Int32 seq);
			void UDPSendPhotoEnd(const Net::SocketUtil::AddressInfo *addr, UInt16 port, Int64 photoTime);
			void UDPSendSetOutput(const Net::SocketUtil::AddressInfo *addr, UInt16 port, UInt8 outputNum, Bool isHigh);

			void SaveDatas();
			void SavePhoto(Int64 cliId, Int64 photoTime, Int32 photoFmt, UInt8 *photoBuff, UOSInt photoSize);

			void LoadData();
			DB::DBTool *DBUse();
			void DBUnuse(DB::DBTool *db);
			void UserPwdCalc(const UTF8Char *userName, const UTF8Char *pwd, UInt8 *buff);
		public:
			SMonitorSvrCore(IO::IWriter *writer, Media::DrawEngine *deng);
			virtual ~SMonitorSvrCore();

			Bool IsError();
			virtual Media::DrawEngine *GetDrawEngine();

			DeviceInfo *DevGet(Int64 cliId, Bool toAdd);
			DeviceInfo *DevAdd(Int64 cliId, const UTF8Char *cpuName, const UTF8Char *platformName);
			Bool DeviceRecvReading(DeviceInfo *dev, Int64 cliTime, OSInt nDigitals, OSInt nReading, OSInt nOutput, Int32 digitalVals, ReadingInfo *readings, Int32 profileId, UInt32 cliIP, UInt16 port);
			Bool DeviceKARecv(DeviceInfo *dev, Int64 kaTime);
			Bool DeviceSetName(Int64 cliId, const UTF8Char *devName);
			Bool DeviceSetPlatform(Int64 cliId, const UTF8Char *platformName);
			Bool DeviceSetCPUName(Int64 cliId, const UTF8Char *cpuName);
			Bool DeviceSetReading(Int64 cliId, Int32 index, UInt16 sensorId, UInt16 readingId, const UTF8Char *readingName);
			Bool DeviceSetVersion(Int64 cliId, Int64 version);
			virtual DeviceInfo *DeviceGet(Int64 cliId);
			virtual Bool DeviceModify(Int64 cliId, const UTF8Char *devName, Int32 flags);
			virtual Bool DeviceSetReadings(DeviceInfo *dev, const UTF8Char *readings);
			virtual Bool DeviceSetDigitals(DeviceInfo *dev, const UTF8Char *digitals);
			virtual OSInt DeviceQueryRec(Int64 cliId, Int64 startTime, Int64 endTime, Data::ArrayList<DevRecord2*> *recList);
			virtual Bool DeviceSetOutput(Int64 cliId, Int32 outputNum, Bool toHigh);

			virtual Bool UserExist();
			virtual Bool UserAdd(const UTF8Char *userName, const UTF8Char *password, Int32 userType);
			virtual Bool UserSetPassword(Int32 userId, const UTF8Char *password);
			virtual LoginInfo *UserLogin(const UTF8Char *userName, const UTF8Char *password);
			virtual void UserFreeLogin(LoginInfo *login);
			virtual OSInt UserGetDevices(Int32 userId, Int32 userType, Data::ArrayList<DeviceInfo*> *devList);
			virtual Bool UserHasDevice(Int32 userId, Int32 userType, Int64 cliId);
			virtual OSInt UserGetList(Data::ArrayList<WebUser*> *userList);
			virtual WebUser *UserGet(Int32 userId);
			virtual Bool UserAssign(Int32 userId, Data::ArrayList<Int64> *devIdList);

			virtual Bool SendCapturePhoto(Int64 cliId);

			virtual void LogRequest(Net::WebServer::IWebRequest *req);

			void UserAgentLog(const UTF8Char *userAgent);
			void UserAgentStore();
			
			void RefererLog(const UTF8Char *referer);
			void RefererStore();
		};
	}
}

#endif
