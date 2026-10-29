#include "Stdafx.h"
#include "AI/AIAgent.h"
#include "AI/AIAgentHandler.h"
#include "AI/AISession.h"
#include "Sync/MutexUsage.h"
#include "Text/JSONBuilder.h"

Text::CStringNN AI::AIAgentHandler::RequestStatusGetName(RequestStatus status)
{
	switch (status)
	{
	case RequestStatus::Pending:
		return CSTR("Pending");
	case RequestStatus::Processing:
		return CSTR("Processing");
	case RequestStatus::Completed:
		return CSTR("Completed");
	case RequestStatus::Failed:
		return CSTR("Failed");
	default:
		return CSTR("Unknown");
	}
}

void __stdcall AI::AIAgentHandler::ProcessThread(NN<Sync::Thread> t)
{
	NN<AIAgentHandler> me = t->GetUserObj().GetNN<AIAgentHandler>();
	UIntOS i;
	UIntOS j;
	NN<AIRequest> aiReq;
	Text::StringBuilderUTF8 sb;
	while (!t->IsStopping())
	{
		Sync::MutexUsage mutUsage(me->reqMut);
		i = 0;
		j = me->requests.GetCount();
		while (i < j)
		{
			aiReq = me->requests.GetItemNoCheck(i);
			if (aiReq->status == RequestStatus::Pending)
			{
				aiReq->status = RequestStatus::Processing;
				mutUsage.EndUse();
				NN<Text::String> answer;
				if (!me->agent->Ask(aiReq->session, aiReq->question->ToCString()).SetTo(answer))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Failed to get answer for question: "));
					sb.Append(aiReq->question);
					me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
					mutUsage.BeginUse();
					aiReq->status = RequestStatus::Failed;
				}
				else
				{
					mutUsage.BeginUse();
					aiReq->answer = answer;
					aiReq->status = RequestStatus::Completed;

					sb.ClearStr();
					sb.AppendC(UTF8STRC("Received answer, contains "));
					UIntOS messageCount = aiReq->session->GetMessageCount();
					sb.AppendUIntOS(messageCount);
					sb.AppendC(UTF8STRC(" messages."));
					me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
				}
			}
			if (aiReq->gotResult)
			{
				FreeAIRequest(aiReq);
				me->requests.RemoveAt(i);
				j--;
			}
			else
			{
				i++;
			}
		}
		mutUsage.EndUse();
		t->Wait(5000);
	}
}

void __stdcall AI::AIAgentHandler::FreeAIRequest(NN<AIRequest> req)
{
	req->question->Release();
	req->session.Delete();
	OPTSTR_DEL(req->answer);
	req.Delete();
}

Bool __stdcall AI::AIAgentHandler::ReqFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<AIAgentHandler> me = NN<AIAgentHandler>::ConvertFrom(svcHdlr);
	NN<Text::String> q;
	req->ParseHTTPForm();
	if (!req->GetHTTPFormStr(CSTR("q")).SetTo(q))
		return false;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Received question: "));
	sb.Append(q);
	me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
	NN<AIRequest> aiReq;
	NEW_CLASSNN(aiReq, AIRequest());
	aiReq->requestId = (UInt64)Data::DateTimeUtil::GetCurrTimeMillis();
	aiReq->session = me->agent->CreateSession(me->systemPrompt->ToCString());
	aiReq->question = q->Clone();
	aiReq->answer = nullptr;
	aiReq->status = RequestStatus::Pending;
	aiReq->gotResult = false;
	{
		Sync::MutexUsage mutUsage(me->reqMut);
		me->requests.Put(aiReq->requestId, aiReq);
	}
	me->thread.Notify();

	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddUInt64(CSTR("id"), aiReq->requestId);
	return me->ResponseJSONStr(req, resp, 0, json.Build());
}

Bool __stdcall AI::AIAgentHandler::SessFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<AIAgentHandler> me = NN<AIAgentHandler>::ConvertFrom(svcHdlr);
	req->ParseHTTPForm();
	NN<AIRequest> aiReq;
	UInt64 id;
	if (!req->GetHTTPFormUInt64(CSTR("id"), id))
	{
		return resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	}
	Sync::MutexUsage mutUsage(me->reqMut);
	if (!me->requests.Get(id).SetTo(aiReq))
	{
		return resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	}

	NN<Text::String> answer;
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddStr(CSTR("question"), aiReq->question->ToCString());
	json.ObjectAddStr(CSTR("status"), RequestStatusGetName(aiReq->status));
	if (aiReq->answer.SetTo(answer))
		json.ObjectAddStr(CSTR("answer"), answer);
	if (aiReq->status == RequestStatus::Completed || aiReq->status == RequestStatus::Failed)
	{
		aiReq->gotResult = true;
	}
	json.ObjectBeginArray(CSTR("session"));
	UIntOS messageCount = aiReq->session->GetMessageCount();
	UIntOS i = 0;
	while (i < messageCount)
	{
		NN<Net::OpenAIMessage> message;
		if (aiReq->session->GetMessage(i).SetTo(message))
		{
			Text::CStringNN role;
			switch (message->GetRole())
			{
			case Net::OpenAIMessageRole::System:
				role = CSTR("system");
				break;
			case Net::OpenAIMessageRole::User:
				role = CSTR("user");
				break;
			case Net::OpenAIMessageRole::Assistant:
				role = CSTR("assistant");
				break;
			case Net::OpenAIMessageRole::Tool:
				role = CSTR("tool");
				break;
			default:
				role = CSTR("unknown");
				break;
			}
			json.ArrayBeginObject();
			json.ObjectAddStr(CSTR("role"), role);
			json.ObjectAddStr(CSTR("content"), message->GetContent());
			NN<Text::String> rawMessage;
			if (message->GetRawMessage().SetTo(rawMessage))
				json.ObjectAddStr(CSTR("rawMessage"), rawMessage);
			NN<Text::String> toolCallId;
			if (message->GetToolCallId().SetTo(toolCallId))
				json.ObjectAddStr(CSTR("toolCallId"), toolCallId);
			json.ObjectEnd();
		}
		i++;
	}
	json.ArrayEnd();
	return me->ResponseJSONStr(req, resp, 0, json.Build());
}

AI::AIAgentHandler::AIAgentHandler(NN<AI::AIAgent> agent, NN<IO::LogTool> log, NN<Text::String> systemPrompt, Text::CStringNN webPath) : WebServiceHandler(webPath), thread(ProcessThread, this, CSTR("AIAgent"))
{
	this->agent = agent;
	this->log = log;
	this->systemPrompt = systemPrompt->Clone();
	this->AddService(CSTR("/api/req"), Net::WebUtil::RequestMethod::HTTP_POST, ReqFunc);
	this->AddService(CSTR("/api/sess"), Net::WebUtil::RequestMethod::HTTP_POST, SessFunc);
	this->thread.Start();
}

AI::AIAgentHandler::~AIAgentHandler()
{
	this->thread.Stop();
	this->systemPrompt->Release();
	this->requests.FreeAll(FreeAIRequest);
}
