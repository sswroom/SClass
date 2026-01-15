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
				UIntOS ofst;
				UIntOS leng;
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
			UIntOS reqDataSize;
			UIntOS reqCurrSize;
			IO::MemoryStream *chunkMStm;
			Optional<Crypto::Cert::X509Cert> remoteCert;

		private:
			void ParseQuery();
			void ParseFormStr(NN<Data::FastStringMapNN<Text::String>> formMap, const UInt8 *buff, UIntOS buffSize);
			void ParseFormPart(UInt8 *data, UIntOS dataSize, UIntOS startOfst);
			Text::CStringNN ParseHeaderVal(UnsafeArray<UTF8Char> headerData, UIntOS dataLen);
		public:
			WebServerRequest(Text::CStringNN requestURI, Net::WebUtil::RequestMethod reqMeth, RequestProtocol reqProto, NN<Net::TCPClient> cli, const Net::SocketUtil::AddressInfo *cliAddr, UInt16 cliPort, UInt16 svrPort);
			virtual ~WebServerRequest();

			void AddHeader(Text::CStringNN name, Text::CStringNN value);
			virtual Optional<Text::String> GetSHeader(Text::CStringNN name) const;
			virtual UnsafeArrayOpt<UTF8Char> GetHeader(UnsafeArray<UTF8Char> sbuff, Text::CStringNN name, UIntOS buffLen) const;
			virtual Bool GetHeaderC(NN<Text::StringBuilderUTF8> sb, Text::CStringNN name) const;
			virtual UIntOS GetHeaderNames(NN<Data::ArrayListStringNN> names) const;
			UIntOS GetHeaderCnt() const;
			Optional<Text::String> GetHeaderName(UIntOS index) const;
			Optional<Text::String> GetHeaderValue(UIntOS index) const;

			virtual NN<Text::String> GetRequestURI() const;
			virtual RequestProtocol GetProtocol() const;
			virtual Optional<Text::String> GetQueryValue(Text::CStringNN name);
			virtual Bool HasQuery(Text::CStringNN name);
			virtual Net::WebUtil::RequestMethod GetReqMethod() const;
			virtual void ParseHTTPForm();
			virtual Optional<Text::String> GetHTTPFormStr(Text::CStringNN name);
			virtual UnsafeArrayOpt<const UInt8> GetHTTPFormFile(Text::CStringNN formName, UIntOS index, UnsafeArrayOpt<UTF8Char> fileName, UIntOS fileNameBuffSize, OptOut<UnsafeArray<UTF8Char>> fileNameEnd, OptOut<UIntOS> fileSize);
			virtual void GetRequestURLBase(NN<Text::StringBuilderUTF8> sb);

			virtual NN<const Net::SocketUtil::AddressInfo> GetClientAddr() const;
			virtual NN<Net::NetConnection> GetNetConn() const;
			virtual UInt16 GetClientPort() const;
			virtual Bool IsSecure() const;
			virtual Optional<Crypto::Cert::X509Cert> GetClientCert();
			virtual UnsafeArrayOpt<const UInt8> GetReqData(OutParam<UIntOS> dataSize);

			Bool IsForwardedSSL() const;

			Bool HasData();
			void DataStart();
			Bool DataStarted();
			Bool DataFull();
			UIntOS DataPut(const UInt8 *data, UIntOS dataSize);

			static UIntOS GetMaxDataSize();
		};
	}
}
#endif
