#ifndef _SM_NET_WEBSERVER_WEBSERVERREQUEST
#define _SM_NET_WEBSERVER_WEBSERVERREQUEST
#include "Data/ArrayListStrUTF8.h"
#include "Data/FastStringMapNN.hpp"
#include "IO/MemoryStream.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
#include "Net/WebServer/WebRequest.h"

namespace Net
{
	namespace WebServer
	{
		class WebServerRequest : public WebRequest
		{
		private:
			typedef struct
			{
				UOSInt ofst;
				UOSInt leng;
				NN<Text::String> formName;
				NN<Text::String> fileName;
			} FormFileInfo;

		private:
			NN<Text::String> requestURI;
			Net::WebUtil::RequestMethod reqMeth;
			Data::FastStringMapNN<Text::String> headers;
			Data::FastStringMapNN<Text::String> *queryMap;
			Net::SocketUtil::AddressInfo cliAddr;
			UInt16 cliPort;
			UInt16 svrPort;
			RequestProtocol reqProto;
			NN<Net::TCPClient> cli;
			Data::FastStringMapNN<Text::String> *formMap;
			Data::ArrayListNN<FormFileInfo> *formFileList;

			UInt8 *reqData;
			UOSInt reqDataSize;
			UOSInt reqCurrSize;
			IO::MemoryStream *chunkMStm;
			Optional<Crypto::Cert::X509Cert> remoteCert;

		private:
			void ParseQuery();
			void ParseFormStr(NN<Data::FastStringMapNN<Text::String>> formMap, const UInt8 *buff, UOSInt buffSize);
			void ParseFormPart(UInt8 *data, UOSInt dataSize, UOSInt startOfst);
			Text::CStringNN ParseHeaderVal(UnsafeArray<UTF8Char> headerData, UOSInt dataLen);
		public:
			WebServerRequest(Text::CStringNN requestURI, Net::WebUtil::RequestMethod reqMeth, RequestProtocol reqProto, NN<Net::TCPClient> cli, const Net::SocketUtil::AddressInfo *cliAddr, UInt16 cliPort, UInt16 svrPort);
			virtual ~WebServerRequest();

			void AddHeader(Text::CStringNN name, Text::CStringNN value);
			virtual Optional<Text::String> GetSHeader(Text::CStringNN name) const;
			virtual UnsafeArrayOpt<UTF8Char> GetHeader(UnsafeArray<UTF8Char> sbuff, Text::CStringNN name, UOSInt buffLen) const;
			virtual Bool GetHeaderC(NN<Text::StringBuilderUTF8> sb, Text::CStringNN name) const;
			virtual UOSInt GetHeaderNames(NN<Data::ArrayListStringNN> names) const;
			UOSInt GetHeaderCnt() const;
			Optional<Text::String> GetHeaderName(UOSInt index) const;
			Optional<Text::String> GetHeaderValue(UOSInt index) const;

			virtual NN<Text::String> GetRequestURI() const;
			virtual RequestProtocol GetProtocol() const;
			virtual Optional<Text::String> GetQueryValue(Text::CStringNN name);
			virtual Bool HasQuery(Text::CStringNN name);
			virtual Net::WebUtil::RequestMethod GetReqMethod() const;
			virtual void ParseHTTPForm();
			virtual Optional<Text::String> GetHTTPFormStr(Text::CStringNN name);
			virtual UnsafeArrayOpt<const UInt8> GetHTTPFormFile(Text::CStringNN formName, UOSInt index, UnsafeArrayOpt<UTF8Char> fileName, UOSInt fileNameBuffSize, OptOut<UnsafeArray<UTF8Char>> fileNameEnd, OptOut<UOSInt> fileSize);
			virtual void GetRequestURLBase(NN<Text::StringBuilderUTF8> sb);

			virtual NN<const Net::SocketUtil::AddressInfo> GetClientAddr() const;
			virtual NN<Net::NetConnection> GetNetConn() const;
			virtual UInt16 GetClientPort() const;
			virtual Bool IsSecure() const;
			virtual Optional<Crypto::Cert::X509Cert> GetClientCert();
			virtual UnsafeArrayOpt<const UInt8> GetReqData(OutParam<UOSInt> dataSize);

			Bool IsForwardedSSL() const;

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
