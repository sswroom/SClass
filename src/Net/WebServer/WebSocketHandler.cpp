#include "Stdafx.h"
#include "Crypto/Hash/SHA1.h"
#include "Net/WebServer/WebSocketHandler.h"
#include "Net/WebServer/WebSocketServerStream.h"
#include "Text/TextBinEnc/Base64Enc.h"

Net::WebServer::WebSocketHandler::WebSocketHandler(IO::StreamHandler *stmHdlr)
{
	this->stmHdlr = stmHdlr;
}

Net::WebServer::WebSocketHandler::~WebSocketHandler()
{

}

Bool Net::WebServer::WebSocketHandler::ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	NN<Text::String> upgrade;
	NN<Text::String> conn;
	NN<Text::String> secWebSocketKey;
	NN<Text::String> secWebSocketProtocol;
	NN<Text::String> secWebSocketVersion;
	if (!req->GetSHeader(CSTR("Upgrade")).SetTo(upgrade) ||
		!req->GetSHeader(CSTR("Connection")).SetTo(conn) ||
		!req->GetSHeader(CSTR("Sec-WebSocket-Key")).SetTo(secWebSocketKey) ||
		!req->GetSHeader(CSTR("Sec-WebSocket-Protocol")).SetTo(secWebSocketProtocol) ||
		!req->GetSHeader(CSTR("Sec-WebSocket-Version")).SetTo(secWebSocketVersion))
	{
		return resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	}
	if (!upgrade->Equals(UTF8STRC("websocket")) || conn->IndexOf(UTF8STRC("Upgrade")) == INVALID_INDEX)
	{
		return resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	}
	if (secWebSocketKey->leng != 24 || !secWebSocketKey->EndsWith(UTF8STRC("==")))
	{
		return resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	}
	Text::StringBuilderUTF8 sb;
	sb.Append(secWebSocketProtocol);
	Text::CStringNN protocol = CSTR("");
	Text::PString parr[2];
	UOSInt nStrs;
	parr[1] = sb;
	while (true)
	{
		nStrs = Text::StrSplitTrimP(parr, 2, parr[1], ',');
		if (parr[0].Equals(UTF8STRC("chat")) || parr[0].Equals(UTF8STRC("mqtt")))
		{
			protocol = parr[0].ToCString();
			break;
		}
		if (nStrs != 2)
			break;
	}
	if (protocol.leng == 0)
	{
		return resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	}
	resp->SetStatusCode(Net::WebStatus::SC_SWITCHING_PROTOCOLS);
	resp->AddHeader(CSTR("Upgrade"), CSTR("websocket"));
	resp->AddHeader(CSTR("Connection"), CSTR("Upgrade"));
	resp->AddHeader(CSTR("Sec-WebSocket-Protocol"), protocol);
	Crypto::Hash::SHA1 sha1;
	UInt8 hash[20];
	sha1.Calc(secWebSocketKey->v, secWebSocketKey->leng);
	sha1.Calc(UTF8STRC("258EAFA5-E914-47DA-95CA-C5AB0DC85B11"));
	sha1.GetValue(hash);
	Text::TextBinEnc::Base64Enc b64;
	sb.ClearStr();
	b64.EncodeBin(sb, hash, 20);
	resp->AddHeader(CSTR("Sec-WebSocket-Accept"), sb.ToCString());

	Net::WebServer::WebSocketServerStream *stm;
	NEW_CLASS(stm, Net::WebServer::WebSocketServerStream(this->stmHdlr, resp));
	return resp->SwitchProtocol(stm);
}
