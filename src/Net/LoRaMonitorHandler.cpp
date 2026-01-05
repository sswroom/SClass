#include "Stdafx.h"
#include "Net/LoRaMonitorCore.h"
#include "Net/LoRaMonitorHandler.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Text/JSONBuilder.h"
#include "Text/StringTool.h"

Bool __stdcall Net::LoRaMonitorHandler::GetGatewayFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<Net::LoRaMonitorHandler> me = NN<WebServiceHandler>::ConvertFrom<Net::LoRaMonitorHandler>(svcHdlr);
	Text::JSONBuilder json(Text::JSONBuilder::ObjectType::OT_ARRAY);
	Sync::MutexUsage mutUsage;
	NN<Data::ReadingListNN<Net::LoRaMonitorCore::GWInfo>> gwList = me->core->GetGWList(mutUsage);
	UOSInt i = 0;
	UOSInt j = gwList->GetCount();
	while (i < j)
	{
		NN<Net::LoRaMonitorCore::GWInfo> gw = gwList->GetItemNoCheck(i);
		json.ArrayBeginObject();
		AppendGW(gw, json);
		json.ObjectEnd();
		i++;
	}
	json.ArrayEnd();
	me->AddResponseHeaders(req, resp);
	resp->AddContentType(CSTR("application/json"));
	return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/json"), json.Build());
}

Bool __stdcall Net::LoRaMonitorHandler::GetGatewayPacketFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<Net::LoRaMonitorHandler> me = NN<WebServiceHandler>::ConvertFrom<Net::LoRaMonitorHandler>(svcHdlr);
	UInt64 gweui;
	if (!req->GetQueryValueU64(CSTR("gweui"), gweui))
	{
		return resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	}
	Sync::MutexUsage mutUsage;
	NN<Net::LoRaMonitorCore::GWInfo> gw;
	if (!me->core->GetGW(gweui, mutUsage).SetTo(gw))
	{
		return resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	Text::JSONBuilder json(Text::JSONBuilder::ObjectType::OT_ARRAY);
	UOSInt i = gw->lastDataBegin;
	while (i != gw->lastDataEnd)
	{
		NN<Net::LoRaMonitorCore::DataPacket> pkt;
		if (gw->lastData[i].SetTo(pkt))
		{
			json.ArrayBeginObject();
			AppendPacket(pkt, json);
			json.ObjectEnd();
		}
		i = (i + 1) & 15;
	}
	me->AddResponseHeaders(req, resp);
	resp->AddContentType(CSTR("application/json"));
	return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/json"), json.Build());
}

void Net::LoRaMonitorHandler::AppendGW(NN<LoRaMonitorCore::GWInfo> gw, NN<Text::JSONBuilder> json)
{
	json->ObjectAddUInt64(CSTR("gweui"), gw->gweui);
	json->ObjectAddStrOpt(CSTR("name"), gw->name);
	json->ObjectAddStrOpt(CSTR("model"), gw->model);
	json->ObjectAddStrOpt(CSTR("sn"), gw->sn);
	json->ObjectAddStrOpt(CSTR("imei"), gw->imei);
	json->ObjectAddStrOpt(CSTR("location"), gw->location);
	json->ObjectAddTSStr(CSTR("lastSeenTime"), gw->lastSeenTime);
}

void Net::LoRaMonitorHandler::AppendPacket(NN<LoRaMonitorCore::DataPacket> pkt, NN<Text::JSONBuilder> json)
{
	json->ObjectAddInt32(CSTR("msgType"), pkt->msgType);
	if (Text::StringTool::IsTextUTF8(Data::ByteArrayR(pkt->msg, pkt->msgSize)))
	{
		json->ObjectAddStr(CSTR("payload"), Text::CStringNN(pkt->msg, pkt->msgSize));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendHexBuff(pkt->msg, pkt->msgSize, ' ', Text::LineBreakType::CRLF);
		json->ObjectAddStr(CSTR("payload"), sb.ToCString());
	}
}

Net::LoRaMonitorHandler::LoRaMonitorHandler(NN<LoRaMonitorCore> core, Text::CStringNN webRoot) : Net::WebServer::WebServiceHandler(webRoot)
{
	this->core = core;
	this->AddService(CSTR("/api/gateway"), Net::WebUtil::RequestMethod::HTTP_GET, GetGatewayFunc);
	this->AddService(CSTR("/api/gateway/packet"), Net::WebUtil::RequestMethod::HTTP_GET, GetGatewayPacketFunc);
}

Net::LoRaMonitorHandler::~LoRaMonitorHandler()
{
}
