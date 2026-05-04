#ifndef _SM_SSWR_AIDEMO_AIDEMOCORE
#define _SM_SSWR_AIDEMO_AIDEMOCORE
#include "Core/Core.h"
#include "DB/DBTool.h"
#include "IO/ConfigFile.h"
#include "IO/ConsoleWriter.h"
#include "IO/ConsoleLogHandler.h"
#include "Map/MapDrawLayer.h"
#include "Map/ShortestPath3D.h"
#include "Map/ESRI/FileGDBDir.h"
#include "Net/OpenAIClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/TCPClientFactory.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AIDemo/AIDemoHandler.h"
#include "Sync/RWMutex.h"
#include "Sync/RWMutexUsage.h"
#include "Sync/Thread.h"

namespace SSWR
{
	namespace AIDemo
	{
		class AIDemoCore
		{
		private:
			IO::ConsoleWriter console;
			IO::LogTool	log;
			Net::OSSocketFactory sockf;
			Net::TCPClientFactory clif;
			Optional<Net::SSLEngine> ssl;
			NN<IO::LogHandler> logHdlr;
			NN<Core::ProgControl> progCtrl;
			Optional<Net::WebServer::WebListener> listener;
			Optional<SSWR::AIDemo::AIDemoHandler> hdlr;

			Optional<Net::OpenAIClient> aiClient;
			Optional<Text::String> dbDDL;
			Optional<Text::String> dbRule;
			Optional<DB::DBTool> db;

		public:
			AIDemoCore(NN<Core::ProgControl> progCtrl);
			~AIDemoCore();

			Bool IsError() const;
			void Run();
			Optional<Net::OpenAIClient> GetAIClient() const { return this->aiClient; }
			Optional<DB::DBTool> GetDB() const { return this->db; }
			Optional<Text::String> GetDBDDL() const { return this->dbDDL; }
			Optional<Text::String> GetDBRule() const { return this->dbRule; }
		};
	}
}
#endif
