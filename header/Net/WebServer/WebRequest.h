#ifndef _SM_NET_WEBSERVER_WEBREQUEST
#define _SM_NET_WEBSERVER_WEBREQUEST
#include "Data/ArrayListStrUTF8.h"
#include "Data/FastStringMap.h"
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
				Text::String *formName;
				Text::String *fileName;
			} FormFileInfo;

		private:
			Text::String *requestURI;
			RequestMethod reqMeth;
			Data::FastStringMap<Text::String*> *headers;
			Data::FastStringMap<Text::String*> *queryMap;
			Net::SocketUtil::AddressInfo cliAddr;
			UInt16 cliPort;
			UInt16 svrPort;
			RequestProtocol reqProto;
			Bool secureConn;
			Data::FastStringMap<Text::String *> *formMap;
			Data::ArrayList<FormFileInfo*> *formFileList;

			UInt8 *reqData;
			UOSInt reqDataSize;
			UOSInt reqCurrSize;
			IO::MemoryStream *chunkMStm;

		private:
			void ParseQuery();
			void ParseFormStr(Data::FastStringMap<Text::String *> *formMap, const UInt8 *buff, UOSInt buffSize);
			void ParseFormPart(UInt8 *data, UOSInt dataSize, UOSInt startOfst);
			const UTF8Char *ParseHeaderVal(Char *headerData);
		public:
			WebRequest(const UTF8Char *requestURI, RequestMethod reqMeth, RequestProtocol reqProto, Bool secureConn, const Net::SocketUtil::AddressInfo *cliAddr, UInt16 cliPort, UInt16 svrPort);
			virtual ~WebRequest();

			void AddHeader(const UTF8Char *name, const UTF8Char *value);
			virtual Text::String *GetSHeader(const UTF8Char *name);
			virtual UTF8Char *GetHeader(UTF8Char *sbuff, const UTF8Char *name, UOSInt buffLen);
			virtual Bool GetHeader(Text::StringBuilderUTF *sb, const UTF8Char *name);
			virtual UOSInt GetHeaderNames(Data::ArrayList<Text::String*> *names);
			UOSInt GetHeaderCnt();
			Text::String *GetHeaderName(UOSInt index);
			Text::String *GetHeaderValue(UOSInt index);

			virtual Text::String *GetRequestURI();
			virtual RequestProtocol GetProtocol();
			virtual Text::String *GetQueryValue(const UTF8Char *name);
			virtual Bool HasQuery(const UTF8Char *name);
			virtual RequestMethod GetReqMethod();
			virtual void ParseHTTPForm();
			virtual Text::String *GetHTTPFormStr(const UTF8Char *name);
			virtual const UInt8 *GetHTTPFormFile(const UTF8Char *formName, UOSInt index, UTF8Char *fileName, UOSInt fileNameBuffSize, UOSInt *fileSize);
			virtual void GetRequestURLBase(Text::StringBuilderUTF *sb);

			virtual const Net::SocketUtil::AddressInfo *GetClientAddr();
			virtual UInt16 GetClientPort();
			virtual Bool IsSecure();
			virtual const UInt8 *GetReqData(UOSInt *dataSize);

			Bool HasData();
			void DataStart();
			Bool DataStarted();
			Bool DataFull();
			UOSInt DataPut(const UInt8 *data, UOSInt dataSize);

			static UOSInt GetMaxDataSize();
		};
	}
}
#endif
