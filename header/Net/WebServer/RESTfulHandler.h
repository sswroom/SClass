#ifndef _SM_NET_WEBSERVER_RESTFULHANDLER
#define _SM_NET_WEBSERVER_RESTFULHANDLER
#include "DB/DBConn.h"
#include "DB/DBTool.h"
#include "IO/LogTool.h"
#include "Net/WebServer/WebStandardHandler.h"

namespace Net
{
	namespace WebServer
	{
		class RESTfulHandler : public Net::WebServer::WebStandardHandler
		{
		private:
			typedef struct
			{
				const UTF8Char *name;
				DB::TableDef *def;
				OSInt rowCnt;
			} TableInfo;
			
		private:
			DB::DBConn *conn;
			DB::DBTool *db;
			IO::LogTool *log;
			Data::StringUTF8Map<TableInfo*> *tables;

		public:
			RESTfulHandler(DB::DBConn *conn, IO::LogTool *log);

		protected:
			virtual ~RESTfulHandler();
		public:
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq);

			void GetTableNames(Data::ArrayList<const UTF8Char*> *tableNames);
			OSInt GetRowCount(const UTF8Char *tableName);
		};
	}
}
#endif
