#include "Stdafx.h"
#include "Core/Core.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebServer/WebConnection.h"
#include <stdio.h>
#include <string>
#include <vector>

class TestClient : public Net::TCPClient
{
public:
	TestClient(NN<Net::SocketFactory> sockf) : Net::TCPClient(sockf, nullptr)
	{
	}

	virtual UIntOS Write(Data::ByteArrayR buff)
	{
		return buff.GetSize();
	}

	virtual Bool GetRemoteAddr(NN<Net::SocketUtil::AddressInfo> addr) const
	{
		Net::SocketUtil::SetAddrInfoV4(addr, 0);
		return true;
	}

	virtual UInt16 GetRemotePort() const { return 0; }
	virtual UInt16 GetLocalPort() const { return 0; }
};

class TestHandler : public Net::WebServer::WebHandler
{
public:
	struct Request
	{
		std::string uri;
		std::string body;
	};
	std::vector<Request> requests;

	virtual void DoWebRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
	{
		NN<Text::String> uri = req->GetRequestURI();
		Request request;
		request.uri.assign((const char*)uri->v.Ptr(), uri->leng);
		UIntOS size = 0;
		UnsafeArray<const UInt8> data;
		if (req->GetReqData(size).SetTo(data))
		{
			request.body.assign((const char*)data.Ptr(), size);
		}
		this->requests.push_back(request);
	}
};

static void Feed(Net::WebServer::WebConnection &conn, Text::CStringNN data)
{
	conn.ReceivedData(data.ToByteArray());
}

static Bool Check(const TestHandler &handler, UIntOS index, const char *uri, const char *body)
{
	return handler.requests.size() > index && handler.requests[index].uri == uri && handler.requests[index].body == body;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Net::OSSocketFactory sockf(true);
	Net::TCPClientFactory clif(sockf);
	TestHandler handler;
	Net::WebServer::WebListener listener(clif, nullptr, handler, 0, 30, 0, 0, CSTR("test"), false, Net::WebServer::KeepAlive::Always, false);
	NN<TestClient> client;
	NEW_CLASSNN(client, TestClient(sockf));
	Net::WebServer::WebConnection conn(clif, nullptr, client, listener, handler, false, Net::WebServer::KeepAlive::Always);

	Feed(conn, CSTR("POST /first HTTP/1.1\r\nContent-Length: 8\r\n\r\nab"));
	Feed(conn, CSTR("c"));
	if (!handler.requests.empty())
	{
		printf("Request completed before its body was received\n");
		return 1;
	}
	Feed(conn, CSTR("defghGET /next HTTP/1.1\r\nHost: test\r\n\r\n"));
	if (handler.requests.size() != 2 || !Check(handler, 0, "/first", "abcdefgh") || !Check(handler, 1, "/next", ""))
	{
		printf("Fragmented body or pipelined request was parsed incorrectly\n");
		return 1;
	}

	Feed(conn, CSTR("POST /second HTTP/1.1\r\nContent-Length: 6\r\n\r\nab\r\ncdGET /third HTTP/1.1\r\n\r\n"));
	if (handler.requests.size() != 4 || !Check(handler, 2, "/second", "ab\r\ncd") || !Check(handler, 3, "/third", ""))
	{
		printf("Complete body or following request was parsed incorrectly\n");
		return 1;
	}

	Feed(conn, CSTR("POST /fourth HTTP/1.1\r\nContent-Length: 4\r\n\r\n"));
	Feed(conn, CSTR("dataGET /fi"));
	Feed(conn, CSTR("fth HTTP/1.1\r\n\r\n"));
	if (handler.requests.size() != 6 || !Check(handler, 4, "/fourth", "data") || !Check(handler, 5, "/fifth", ""))
	{
		printf("Fragmented next request was parsed incorrectly\n");
		return 1;
	}
	Feed(conn, CSTR(""));
	if (handler.requests.size() != 6)
	{
		printf("Empty input produced an unexpected request\n");
		return 1;
	}
	return 0;
}
