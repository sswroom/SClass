#include "Stdafx.h"
#include "Net/LoRaMonitorCore.h"
#include "Net/LoRaMonitorHandler.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Text/JSONBuilder.h"

Bool __stdcall Net::LoRaMonitorHandler::GetGatewaysFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
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

Net::LoRaMonitorHandler::LoRaMonitorHandler(NN<LoRaMonitorCore> core)
{
	this->core = core;
	this->AddService(CSTR("/api/gateways"), Net::WebUtil::RequestMethod::HTTP_GET, GetGatewaysFunc);
}

Net::LoRaMonitorHandler::~LoRaMonitorHandler()
{
}
