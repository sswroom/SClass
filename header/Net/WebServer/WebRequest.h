#ifndef _SM_NET_WEBSERVER_WEBREQUEST
#define _SM_NET_WEBSERVER_WEBREQUEST
#include "Data/ArrayListStrUTF8.h"
#include "Data/StringUTF8Map.h"
#include "IO/MemoryStream.h"
#include "Net/SocketFactory.h"
#include "Net/WebServer/IWebRequest.h"

namespace Net
{
	namespace WebServer
	{
		class WebRequest : public IWebRequest
		{
		private:
			typedef struct
			{
				UOSInt ofst;
				UOSInt leng;
				const UTF8Char *formName;
				const UTF8Char *fileName;
			} FormFileInfo;
		private:
			const UTF8Char *requestURI;
			RequestMethod reqMeth;
			Data::ArrayListStrUTF8 *headerNames;
			Data::ArrayListStrUTF8 *headerVals;
			Data::StringUTF8Map<const UTF8Char *> *queryMap;
			Net::SocketUtil::AddressInfo cliAddr;
			UInt16 cliPort;
			RequestProtocol reqProto;
			Data::StringUTF8Map<const UTF8Char *> *formMap;
			Data::ArrayList<FormFileInfo*> *formFileList;

			UInt8 *reqData;
			UOSInt reqDataSize;
			UOSInt reqCurrSize;
			IO::MemoryStream *chunkMStm;

		private:
			void ParseQuery();
			void ParseFormStr(Data::StringUTF8Map<const UTF8Char *> *formMap, const UInt8 *buff, UOSInt buffSize);
			void ParseFormPart(UInt8 *data, UOSInt dataSize, UOSInt startOfst);
			const UTF8Char *ParseHeaderVal(Char *headerData);
			const UTF8Char *GetSHeader(const UTF8Char *name);
		public:
			WebRequest(const UTF8Char *requestURI, RequestMethod reqMeth, RequestProtocol reqProto, const Net::SocketUtil::AddressInfo *cliAddr, UInt16 cliPort);
			virtual ~WebRequest();

			void AddHeader(const UTF8Char *name, const UTF8Char *value);
			virtual UTF8Char *GetHeader(UTF8Char *sbuff, const UTF8Char *name, UOSInt buffLen);
			virtual Bool GetHeader(Text::StringBuilderUTF *sb, const UTF8Char *name);
			virtual UOSInt GetHeaderNames(Data::ArrayList<const UTF8Char*> *names);
			UOSInt GetHeaderCnt();
			const UTF8Char *GetHeaderName(UOSInt index);
			const UTF8Char *GetHeaderValue(UOSInt index);

			virtual const UTF8Char *GetRequestURI();
			virtual RequestProtocol GetProtocol();
			virtual const UTF8Char *GetQueryValue(const UTF8Char *name);
			virtual Bool HasQuery(const UTF8Char *name);
			virtual RequestMethod GetReqMethod();
			virtual void ParseHTTPForm();
			virtual const UTF8Char *GetHTTPFormStr(const UTF8Char *name);
			virtual const UInt8 *GetHTTPFormFile(const UTF8Char *formName, UOSInt index, UTF8Char *fileName, UOSInt fileNameBuffSize, UOSInt *fileSize);

			virtual const Net::SocketUtil::AddressInfo *GetClientAddr();
			virtual UInt16 GetClientPort();
			virtual const UInt8 *GetReqData(UOSInt *dataSize);

			Bool HasData();
			void DataStart();
			Bool DataStarted();
			Bool DataFull();
			OSInt DataPut(const UInt8 *data, OSInt dataSize);

			static OSInt GetMaxDataSize();
		};
	}
}
#endif
