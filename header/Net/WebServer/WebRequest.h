#ifndef _SM_NET_WEBSERVER_WEBREQUEST
#define _SM_NET_WEBSERVER_WEBREQUEST
#include "Data/ArrayListStrUTF8.h"
#include "Data/FastStringMap.h"
#include "IO/MemoryStream.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
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
				NotNullPtr<Text::String> formName;
				NotNullPtr<Text::String> fileName;
			} FormFileInfo;

		private:
			NotNullPtr<Text::String> requestURI;
			Net::WebUtil::RequestMethod reqMeth;
			Data::FastStringMap<Text::String*> headers;
			Data::FastStringMap<Text::String*> *queryMap;
			Net::SocketUtil::AddressInfo cliAddr;
			UInt16 cliPort;
			UInt16 svrPort;
			RequestProtocol reqProto;
			Net::TCPClient *cli;
			Data::FastStringMap<Text::String *> *formMap;
			Data::ArrayList<FormFileInfo*> *formFileList;

			UInt8 *reqData;
			UOSInt reqDataSize;
			UOSInt reqCurrSize;
			IO::MemoryStream *chunkMStm;
			Crypto::Cert::X509Cert *remoteCert;

		private:
			void ParseQuery();
			void ParseFormStr(Data::FastStringMap<Text::String *> *formMap, const UInt8 *buff, UOSInt buffSize);
			void ParseFormPart(UInt8 *data, UOSInt dataSize, UOSInt startOfst);
			Text::CString ParseHeaderVal(UTF8Char *headerData, UOSInt dataLen);
		public:
			WebRequest(Text::CString requestURI, Net::WebUtil::RequestMethod reqMeth, RequestProtocol reqProto, Net::TCPClient *cli, const Net::SocketUtil::AddressInfo *cliAddr, UInt16 cliPort, UInt16 svrPort);
			virtual ~WebRequest();

			void AddHeader(Text::CString name, Text::CString value);
			virtual Text::String *GetSHeader(Text::CString name);
			virtual UTF8Char *GetHeader(UTF8Char *sbuff, Text::CString name, UOSInt buffLen);
			virtual Bool GetHeaderC(Text::StringBuilderUTF8 *sb, Text::CString name);
			virtual UOSInt GetHeaderNames(Data::ArrayList<Text::String*> *names);
			UOSInt GetHeaderCnt();
			Text::String *GetHeaderName(UOSInt index);
			Text::String *GetHeaderValue(UOSInt index);

			virtual NotNullPtr<Text::String> GetRequestURI() const;
			virtual RequestProtocol GetProtocol() const;
			virtual Text::String *GetQueryValue(Text::CString name);
			virtual Bool HasQuery(Text::CString name);
			virtual Net::WebUtil::RequestMethod GetReqMethod() const;
			virtual void ParseHTTPForm();
			virtual Text::String *GetHTTPFormStr(Text::CString name);
			virtual const UInt8 *GetHTTPFormFile(Text::CString formName, UOSInt index, UTF8Char *fileName, UOSInt fileNameBuffSize, UTF8Char **fileNameEnd, UOSInt *fileSize);
			virtual void GetRequestURLBase(Text::StringBuilderUTF8 *sb);

			virtual const Net::SocketUtil::AddressInfo *GetClientAddr() const;
			virtual Net::NetConnection *GetNetConn() const;
			virtual UInt16 GetClientPort() const;
			virtual Bool IsSecure() const;
			virtual Crypto::Cert::X509Cert *GetClientCert();
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
