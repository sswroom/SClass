#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/HTTPClient.h"
#include "Net/MIME.h"
#include "Net/OpenAIClient.h"
#include "Text/JSText.h"
#include "Text/TextBinEnc/Base64Enc.h"

//#define VERBOSE

Net::OpenAIClient::OpenAIClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN apiURL, Text::CStringNN apiKey)
{
	this->clif = clif;
	this->ssl = ssl;
	if (apiURL.EndsWith('/'))
	{
		this->apiURL = Text::String::New(apiURL.v, apiURL.leng - 1);
	}
	else
	{
		this->apiURL = Text::String::New(apiURL);
	}
	this->apiKey = Text::String::New(apiKey);
	this->modelIndex = 0;
	Text::StringBuilderUTF8 sb;
	sb.Append(this->apiURL);
	sb.Append(CSTR("/models"));
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (!cli->IsError())
	{
		sb.ClearStr();
		sb.Append(CSTR("Bearer "));
		sb.Append(this->apiKey);
		cli->AddHeaderC(CSTR("Authorization"), sb.ToCString());
		sb.ClearStr();
		if (cli->ReadAllContent(sb, 8192, 1048576))
		{
			NN<Text::JSONBase> jsonObj;
			if (Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(jsonObj))
			{
				NN<Text::JSONArray> arr;
				if (jsonObj->GetValueArray(CSTR("data")).SetTo(arr))
				{
					UIntOS i = 0;
					UIntOS j = arr->GetArrayLength();
					while (i < j)
					{
						NN<Text::JSONObject> obj;
						if (arr->GetArrayObject(i).SetTo(obj))
						{
							NN<Text::String> s;
							if (obj->GetValueString(CSTR("id")).SetTo(s))
							{
								this->modelList.Add(s->Clone());
							}
						}
						i++;
					}
				}
				jsonObj->EndUse();
			}
		}
	}
	cli.Delete();
}

Net::OpenAIClient::~OpenAIClient()
{
	this->apiKey->Release();
	this->apiURL->Release();
	this->modelList.FreeAll();
}

Bool Net::OpenAIClient::IsError() const
{
	return this->modelList.GetCount() == 0;
}

Bool Net::OpenAIClient::SetModel(Text::CStringNN model)
{
	UIntOS i = 0;
	UIntOS j = this->modelList.GetCount();
	while (i < j)
	{
		if (this->modelList.GetItemNoCheck(i)->Equals(model))
		{
			this->modelIndex = i;
			return true;
		}
		i++;
	}
	return false;
}

UIntOS Net::OpenAIClient::GetModelCount() const
{
	return this->modelList.GetCount();
}

Optional<Text::String> Net::OpenAIClient::GetModelName(UIntOS index) const
{
	return this->modelList.GetItem(index);
}

Text::CStringNN Net::OpenAIClient::GetCurrModel() const
{
	return Text::String::OrEmpty(this->modelList.GetItem(this->modelIndex))->ToCString();
}

NN<Net::OpenAIResult> Net::OpenAIClient::SendChatCompletion(NN<OpenAIChatCompletion> chat)
{
	NN<OpenAIResult> ret;
	Text::StringBuilderUTF8 sb;
	sb.Append(this->apiURL);
	sb.Append(CSTR("/chat/completions"));
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, true);
	if (cli->IsError())
	{
		NEW_CLASSNN(ret, Net::OpenAIResult(0, CSTR("Error in connecting to server")));
		cli.Delete();
		return ret;
	}
	sb.ClearStr();
	sb.Append(CSTR("Bearer "));
	sb.Append(this->apiKey);
	cli->AddHeaderC(CSTR("Authorization"), sb.ToCString());
	cli->AddHeaderC(CSTR("Content-Type"), CSTR("application/json"));
	sb.ClearStr();
	chat->ToJSON(sb);
	cli->AddContentLength(sb.GetLength());
#if defined(VERBOSE)
	printf("OpenAIClient.SendChatCompletion: %s\n", sb.v.Ptr());
#endif
	if (!cli->Write(sb.ToByteArray()))
	{
		NEW_CLASSNN(ret, Net::OpenAIResult(0, CSTR("Error in sending data to server")));
		cli.Delete();
		return ret;
	}
	sb.ClearStr();
	if (!cli->ReadAllContent(sb, 8192, 1048576))
	{
		NEW_CLASSNN(ret, Net::OpenAIResult(0, CSTR("Error in reading data from server")));
		cli.Delete();
		return ret;
	}
#if defined(VERBOSE)
	printf("\n%s\n", sb.v.Ptr());
#endif
	UInt32 statusCode = (UInt32)cli->GetRespStatus();
	cli.Delete();
	NEW_CLASSNN(ret, Net::OpenAIResult(statusCode, sb.ToCString()));
	return ret;
}

NN<Net::OpenAIResult> Net::OpenAIClient::SendResponses(NN<OpenAIResponse> resp)
{
	NN<OpenAIResult> ret;
	Text::StringBuilderUTF8 sb;
	sb.Append(this->apiURL);
	sb.Append(CSTR("/responses"));
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, true);
	if (cli->IsError())
	{
		NEW_CLASSNN(ret, Net::OpenAIResult(0, CSTR("Error in connecting to server")));
		cli.Delete();
		return ret;
	}
	sb.ClearStr();
	sb.Append(CSTR("Bearer "));
	sb.Append(this->apiKey);
	cli->AddHeaderC(CSTR("Authorization"), sb.ToCString());
	cli->AddHeaderC(CSTR("Content-Type"), CSTR("application/json"));
	sb.ClearStr();
	resp->ToJSON(sb);
	cli->AddContentLength(sb.GetLength());
#if defined(VERBOSE)
	printf("OpenAIClient.SendResponses: %s\n", sb.v.Ptr());
#endif
	if (!cli->Write(sb.ToByteArray()))
	{
		NEW_CLASSNN(ret, Net::OpenAIResult(0, CSTR("Error in sending data to server")));
		cli.Delete();
		return ret;
	}
	cli->EndRequest(nullptr, nullptr);
	Text::CStringNN contentType;
#if defined(VERBOSE)
	if (cli->GetRespHeader(CSTR("Content-Type")).SetTo(contentType))
	{
		printf("OpenAIClient.SendResponses: Content-Type: %s\n", contentType.v.Ptr());
	}
	else
	{
		printf("OpenAIClient.SendResponses: Content-Type: null\n");
	}
#endif
	if (cli->GetRespHeader(CSTR("Content-Type")).SetTo(contentType) && contentType.StartsWith(CSTR("text/event-stream")))
	{
		NEW_CLASSNN(ret, Net::OpenAIStreamResult(cli));
		NN<Net::OpenAIStreamResult>::ConvertFrom(ret)->NextEvent();
		return ret;
	}
	else
	{
		sb.ClearStr();
		if (!cli->ReadAllContent(sb, 8192, 1048576))
		{
			NEW_CLASSNN(ret, Net::OpenAIResult(0, CSTR("Error in reading data from server")));
			cli.Delete();
			return ret;
		}
#if defined(VERBOSE)
		printf("\n%s\n", sb.v.Ptr());
#endif
		UInt32 statusCode = (UInt32)cli->GetRespStatus();
		cli.Delete();
		NEW_CLASSNN(ret, Net::OpenAIResult(statusCode, sb.ToCString()));
		return ret;
	}
}

NN<Net::OpenAIResult> Net::OpenAIClient::GetResponseResult(Text::CStringNN responseId)
{
	NN<Net::OpenAIResult> ret;
	Text::StringBuilderUTF8 sb;
	sb.Append(this->apiURL);
	sb.Append(CSTR("/responses/"));
	sb.Append(responseId);
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli->IsError())
	{
		NEW_CLASSNN(ret, Net::OpenAIResult(0, CSTR("Error in connecting to server")));
		cli.Delete();
		return ret;
	}
	sb.ClearStr();
	sb.Append(CSTR("Bearer "));
	sb.Append(this->apiKey);
	cli->AddHeaderC(CSTR("Authorization"), sb.ToCString());
	sb.ClearStr();
	if (!cli->ReadAllContent(sb, 8192, 1048576))
	{
		NEW_CLASSNN(ret, Net::OpenAIResult(0, CSTR("Error in reading data from server")));
		cli.Delete();
		return ret;
	}
#if defined(VERBOSE)
	printf("\n%s\n", sb.v.Ptr());
#endif
	UInt32 statusCode = (UInt32)cli->GetRespStatus();
	cli.Delete();
	NEW_CLASSNN(ret, Net::OpenAIResult(statusCode, sb.ToCString()));
	return ret;
}

Net::OpenAIMessage::OpenAIMessage(NN<OpenAIMessage> msg)
{
	this->role = msg->role;
	this->content = msg->content->Clone();
	this->next = msg->next;
}

Net::OpenAIMessage::OpenAIMessage(OpenAIMessageRole role, Text::CStringNN content)
{
	this->role = role;
	this->content = Text::String::New(content);
	this->next = nullptr;
}

Net::OpenAIMessage::~OpenAIMessage()
{
	this->content->Release();
	this->next.Delete();
}

NN<Net::OpenAIMessage> Net::OpenAIMessage::Clone() const
{
	NN<Net::OpenAIMessage> msg;
	NEW_CLASSNN(msg, Net::OpenAIMessage(*this));
	return msg;
}

Net::OpenAIMessageRole Net::OpenAIMessage::GetRole() const
{
	return this->role;
}

Text::CStringNN Net::OpenAIMessage::GetContent() const
{
	return this->content->ToCString();
}

Optional<Net::OpenAIMessage> Net::OpenAIMessage::GetNext() const
{
	return this->next;
}

void Net::OpenAIMessage::SetNext(Optional<OpenAIMessage> next)
{
	NN<OpenAIMessage> msg;
	this->next.Delete();
	if (next.SetTo(msg))
	{
		this->next = msg->Clone();
	}
}

void Net::OpenAIMessage::ToJSON(NN<Text::StringBuilderUTF8> sb) const
{
	sb->Append(CSTR("{\"role\":\""));
	switch (this->role)
	{
	case OpenAIMessageRole::System:
		sb->Append(CSTR("system"));
		break;
	case OpenAIMessageRole::User:
		sb->Append(CSTR("user"));
		break;
	case OpenAIMessageRole::Assistant:
		sb->Append(CSTR("assistant"));
		break;
	}
	sb->Append(CSTR("\",\"content\":"));
	Text::JSText::ToJSTextDQuote(sb, this->content->v);
	sb->AppendUTF8Char('}');
}

Net::OpenAIChatCompletion::OpenAIChatCompletion(Text::CStringNN model)
{
	this->model = model;
	this->firstMessage = nullptr;
	this->lastMessage = nullptr;
}

Net::OpenAIChatCompletion::~OpenAIChatCompletion()
{
}

Text::CStringNN Net::OpenAIChatCompletion::GetModel() const
{
	return this->model;
}

Optional<Net::OpenAIMessage> Net::OpenAIChatCompletion::GetFirstMessage() const
{
	return this->firstMessage;
}

void Net::OpenAIChatCompletion::AddMessage(OpenAIMessageRole role, Text::CStringNN content)
{
	NN<OpenAIMessage> lastMessage;
	NN<OpenAIMessage> msg;
	NEW_CLASSNN(msg, OpenAIMessage(role, content));
	if (this->lastMessage.SetTo(lastMessage))
	{
		lastMessage->SetNext(msg);
	}
	else
	{
		this->firstMessage = msg;
	}
	this->lastMessage = msg;
}

void Net::OpenAIChatCompletion::ToJSON(NN<Text::StringBuilderUTF8> sb) const
{
	sb->Append(CSTR("{\"model\":"));
	Text::JSText::ToJSTextDQuote(sb, this->model.v);
	sb->Append(CSTR(",\"messages\":["));
	NN<OpenAIMessage> msg;
	if (this->firstMessage.SetTo(msg))
	{
		msg->ToJSON(sb);
		while (msg->GetNext().SetTo(msg))
		{
			sb->AppendUTF8Char(',');
			msg->ToJSON(sb);
		}
	}
	sb->Append(CSTR("]}"));
}

Net::OpenAIResponse::OpenAIResponse(Text::CStringNN model, Text::CStringNN userInput, Text::CString systemInput)
{
	this->model = model;
	this->userInput = userInput;
	this->systemInput = systemInput;
	this->previousResponseId = nullptr;
	this->reasoningEffort = OpenAIReasoningEffort::Default;
	this->maxTokens = 0;
	this->temperature = NAN;
	this->topP = NAN;
	this->background = false;
	this->stream = false;
}

Net::OpenAIResponse::~OpenAIResponse()
{
	this->fileList.FreeAll();
	this->fileNameList.FreeAll();
}

Text::CStringNN Net::OpenAIResponse::GetModel() const
{
	return this->model;
}

Text::CStringNN Net::OpenAIResponse::GetUserInput() const
{
	return this->userInput;
}

Text::CString Net::OpenAIResponse::GetSystemInput() const
{
	return this->systemInput;
}

void Net::OpenAIResponse::SetPreviousResponseId(Text::CStringNN responseId)
{
	this->previousResponseId = responseId;
}

void Net::OpenAIResponse::SetReasoningEffort(OpenAIReasoningEffort effort)
{
	this->reasoningEffort = effort;
}

void Net::OpenAIResponse::SetMaxTokens(UIntOS maxTokens)
{
	this->maxTokens = maxTokens;
}

void Net::OpenAIResponse::SetTemperature(Double temperature)
{
	this->temperature = temperature;
}

void Net::OpenAIResponse::SetTopP(Double topP)
{
	this->topP = topP;
}

void Net::OpenAIResponse::SetBackground(Bool background)
{
	this->background = background;
}

void Net::OpenAIResponse::SetStream(Bool stream)
{
	this->stream = stream;
}

Bool Net::OpenAIResponse::AddFile(Text::CStringNN filePath)
{
	IO::FileStream fs(filePath, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	UIntOS i;
	IO::MemoryStream mstm;
	fs.ReadToEnd(mstm, 10485760);
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("data:"));
	sb.Append(Net::MIME::GetMIMEFromFileName(filePath.v, filePath.leng));
	sb.Append(CSTR(";base64,"));
	Text::TextBinEnc::Base64Enc b64;
	b64.EncodeBin(sb, UnsafeArray<const UInt8>(mstm.GetBuff()), (UIntOS)mstm.GetLength());
	this->fileList.Add(Text::String::New(sb.ToCString()));
	i = filePath.LastIndexOf(IO::Path::PATH_SEPERATOR);
	this->fileNameList.Add(Text::String::New(filePath.Substring(i + 1)));
	return true;
}

void Net::OpenAIResponse::ToJSON(NN<Text::StringBuilderUTF8> sb) const
{
	Text::CStringNN systemInput;
	sb->Append(CSTR("{\"model\":"));
	Text::JSText::ToJSTextDQuote(sb, this->model.v);
	if (this->maxTokens > 0)
	{
		sb->Append(CSTR(",\"max_tokens\":"));
		sb->AppendUIntOS(this->maxTokens);
	}
	if (!Math::IsNAN(this->temperature))
	{
		sb->Append(CSTR(",\"temperature\":"));
		sb->AppendDouble(this->temperature);
	}
	if (!Math::IsNAN(this->topP))
	{
		sb->Append(CSTR(",\"top_p\":"));
		sb->AppendDouble(this->topP);
	}
	if (this->background)
	{
		sb->Append(CSTR(",\"background\":true"));
	}
	if (this->stream)
	{
		sb->Append(CSTR(",\"stream\":true"));
	}
	sb->Append(CSTR(",\"input\":"));
	if (this->fileList.GetCount() > 0)
	{
		sb->AppendUTF8Char('[');
		if (this->systemInput.SetTo(systemInput))
		{
			sb->Append(CSTR("{\"role\":\"system\",\"type\":\"message\",\"content\":"));
			Text::JSText::ToJSTextDQuote(sb, systemInput.v);
		}
		sb->Append(CSTR("{\"role\":\"user\",\"type\":\"message\",\"content\":[{\"type\":\"text\",\"text\":"));
		Text::JSText::ToJSTextDQuote(sb, this->userInput.v);
		sb->AppendUTF8Char('}');
		UIntOS i = 0;
		UIntOS j = this->fileList.GetCount();
		while (i < j)
		{
			sb->AppendUTF8Char(',');
			sb->Append(CSTR("{\"type\":\"file\",\"filename\":"));
			Text::JSText::ToJSTextDQuote(sb, this->fileNameList.GetItemNoCheck(i)->v);
			sb->Append(CSTR(",\"file_data\":\""));
			sb->Append(this->fileList.GetItemNoCheck(i));
			sb->Append(CSTR("\"}"));
			i++;
		}
		sb->Append(CSTR("]}]"));
	}
	else
	{
		Text::JSText::ToJSTextDQuote(sb, this->userInput.v);
		if (this->systemInput.SetTo(systemInput))
		{
			sb->Append(CSTR(",\"instructions\":"));
			Text::JSText::ToJSTextDQuote(sb, systemInput.v);
		}
	}
	Text::CStringNN prevId;
	if (this->previousResponseId.SetTo(prevId))
	{
		sb->Append(CSTR(",\"previous_response_id\":"));
		Text::JSText::ToJSTextDQuote(sb, prevId.v);
	}
	if (this->reasoningEffort != OpenAIReasoningEffort::Default)
	{
		sb->Append(CSTR(",\"reasoning\":{\"effort\":\""));
		switch (this->reasoningEffort)
		{
		case OpenAIReasoningEffort::None:
			sb->Append(CSTR("none"));
			break;
		case OpenAIReasoningEffort::Low:
			sb->Append(CSTR("low"));
			break;
		case OpenAIReasoningEffort::Medium:
			sb->Append(CSTR("medium"));
			break;
		case OpenAIReasoningEffort::High:
			sb->Append(CSTR("high"));
			break;
		case OpenAIReasoningEffort::Default:
		default:
			sb->Append(CSTR("default"));
			break;
		}
		sb->Append(CSTR("\"}"));
	}
	sb->AppendUTF8Char('}');
}

Optional<Text::String> Net::OpenAIResult::GetJSONStr(Text::CStringNN name) const
{
	NN<Text::JSONBase> jsonObj;
	if (this->responseJSON.SetTo(jsonObj))
	{
		return jsonObj->GetValueString(name);
	}
	return nullptr;
}

Bool Net::OpenAIResult::GetJSONInt64(Text::CStringNN name, OutParam<Int64> value) const
{
	NN<Text::JSONBase> jsonObj;
	if (this->responseJSON.SetTo(jsonObj))
	{
		return jsonObj->GetValueAsInt64(name, value);
	}
	return false;
}

Bool Net::OpenAIResult::GetJSONBool(Text::CStringNN name) const
{
	NN<Text::JSONBase> jsonObj;
	if (this->responseJSON.SetTo(jsonObj))
	{
		return jsonObj->GetValueAsBool(name);
	}
	return false;
}

Double Net::OpenAIResult::GetJSONDoubleOrNAN(Text::CStringNN name) const
{
	NN<Text::JSONBase> jsonObj;
	if (this->responseJSON.SetTo(jsonObj))
	{
		return jsonObj->GetValueAsDoubleOrNAN(name);
	}
	return NAN;
}

Net::OpenAIResult::OpenAIResult(UInt32 statusCode, Text::CStringNN responseText)
{
	this->statusCode = statusCode;
	this->responseText = Text::String::New(responseText);
	this->responseJSON = Text::JSONBase::ParseJSONStr(responseText);
}

Net::OpenAIResult::~OpenAIResult()
{
	this->responseText->Release();
	NN<Text::JSONBase> jsonObj;
	if (this->responseJSON.SetTo(jsonObj))
	{
		jsonObj->EndUse();
	}
}

Optional<Text::String> Net::OpenAIResult::GetID() const
{
	return this->GetJSONStr(CSTR("id"));
}

Optional<Text::String> Net::OpenAIResult::GetObject() const
{
	return this->GetJSONStr(CSTR("object"));
}

Optional<Text::String> Net::OpenAIResult::GetStatus() const
{
	return this->GetJSONStr(CSTR("status"));
}

Optional<Text::String> Net::OpenAIResult::GetModel() const
{
	return this->GetJSONStr(CSTR("model"));
}

Bool Net::OpenAIResult::GetCreatedAt(OutParam<Int64> createdAt) const
{
	return this->GetJSONInt64(CSTR("created_at"), createdAt);
}

Bool Net::OpenAIResult::GetCompletedAt(OutParam<Int64> completedAt) const
{
	return this->GetJSONInt64(CSTR("completed_at"), completedAt);
}

Optional<Text::String> Net::OpenAIResult::GetPreviousResponseId() const
{
	return this->GetJSONStr(CSTR("previous_response_id"));
}

Optional<Text::String> Net::OpenAIResult::GetInstructions() const
{
	return this->GetJSONStr(CSTR("instructions"));
}

Optional<Text::String> Net::OpenAIResult::GetError() const
{
	return this->GetJSONStr(CSTR("error"));
}

Optional<Text::String> Net::OpenAIResult::GetToolChoice() const
{
	return this->GetJSONStr(CSTR("tool_choice"));
}

Optional<Text::String> Net::OpenAIResult::GetTruncation() const
{
	return this->GetJSONStr(CSTR("truncation"));
}

Bool Net::OpenAIResult::IsParallelToolCalls() const
{
	return this->GetJSONBool(CSTR("parallel_tool_calls"));
}

Double Net::OpenAIResult::GetTopP() const
{
	return this->GetJSONDoubleOrNAN(CSTR("top_p"));
}

Double Net::OpenAIResult::GetPresencePenalty() const
{
	return this->GetJSONDoubleOrNAN(CSTR("presence_penalty"));
}

Double Net::OpenAIResult::GetFrequencyPenalty() const
{
	return this->GetJSONDoubleOrNAN(CSTR("frequency_penalty"));
}

Double Net::OpenAIResult::GetTopLogprobs() const
{
	return this->GetJSONDoubleOrNAN(CSTR("top_logprobs"));
}

Double Net::OpenAIResult::GetTemperature() const
{
	return this->GetJSONDoubleOrNAN(CSTR("temperature"));
}

Bool Net::OpenAIResult::GetInputTokens(OutParam<Int64> inputTokens) const
{
	return this->GetJSONInt64(CSTR("usage.input_tokens"), inputTokens);
}

Bool Net::OpenAIResult::GetOutputTokens(OutParam<Int64> outputTokens) const
{
	return this->GetJSONInt64(CSTR("usage.output_tokens"), outputTokens);
}

Bool Net::OpenAIResult::GetTotalTokens(OutParam<Int64> totalTokens) const
{
	return this->GetJSONInt64(CSTR("usage.total_tokens"), totalTokens);
}

Bool Net::OpenAIResult::GetInputCachedTokens(OutParam<Int64> inputCachedTokens) const
{
	return this->GetJSONInt64(CSTR("usage.input_tokens_details.cached_tokens"), inputCachedTokens);
}

Bool Net::OpenAIResult::GetOutputReasoningTokens(OutParam<Int64> outputReasoningTokens) const
{
	return this->GetJSONInt64(CSTR("usage.output_tokens_details.reasoning_tokens"), outputReasoningTokens);
}

Bool Net::OpenAIResult::IsStore() const
{
	return this->GetJSONBool(CSTR("store"));
}

Bool Net::OpenAIResult::IsBackground() const
{
	return this->GetJSONBool(CSTR("background"));
}

Bool Net::OpenAIResult::IsStream() const
{
	return false;
}

void Net::OpenAIResult::LockAccess(NN<Sync::MutexUsage> mutUsage)
{
}

Optional<Text::String> Net::OpenAIResult::GetServiceTier() const
{
	return this->GetJSONStr(CSTR("service_tier"));
}

UInt32 Net::OpenAIResult::GetStatusCode() const
{
	return this->statusCode;
}

NN<Text::String> Net::OpenAIResult::GetResponseText() const
{
	return this->responseText;
}

Text::CString Net::OpenAIResult::GetOutputReasoning() const
{
	NN<Text::JSONBase> jsonObj;
	if (!this->responseJSON.SetTo(jsonObj))
	{
		return nullptr;
	}

	NN<Text::JSONArray> arr;
	if (!jsonObj->GetValueArray(CSTR("output")).SetTo(arr))
	{
		return nullptr;
	}
	NN<Text::String> s;
	UIntOS i = 0;
	UIntOS j = arr->GetArrayLength();
	while (i < j)
	{
		NN<Text::JSONObject> obj;
		if (arr->GetArrayObject(i).SetTo(obj))
		{
			if (obj->GetObjectString(CSTR("type")).SetTo(s) && s->Equals(CSTR("reasoning")))
			{
				Optional<Text::String> s = obj->GetValueString(CSTR("content[0].text"));
				return OPTSTR_CSTR(s);
			}
		}
		i++;
	}
	return nullptr;
}

Text::CStringNN Net::OpenAIResult::GetOutputMessage() const
{
	NN<Text::JSONBase> jsonObj;
	if (!this->responseJSON.SetTo(jsonObj))
	{
		return this->responseText->ToCString();
	}

	NN<Text::JSONArray> arr;
	if (!jsonObj->GetValueArray(CSTR("output")).SetTo(arr))
	{
		return this->responseText->ToCString();
	}
	NN<Text::String> s;
	UIntOS i = 0;
	UIntOS j = arr->GetArrayLength();
	while (i < j)
	{
		NN<Text::JSONObject> obj;
		if (arr->GetArrayObject(i).SetTo(obj))
		{
			if (obj->GetObjectString(CSTR("type")).SetTo(s) && s->Equals(CSTR("message")))
			{
				if (obj->GetValueString(CSTR("content[0].text")).SetTo(s))
				{
					return s->ToCString();
				}
			}
		}
		i++;
	}
	return this->responseText->ToCString();
}

Bool Net::OpenAIResult::IsQueuedOrInProgress() const
{
	NN<Text::JSONBase> jsonObj;
	if (!this->responseJSON.SetTo(jsonObj))
	{
		return false;
	}
	NN<Text::String> s;
	if (jsonObj->GetValueString(CSTR("status")).SetTo(s))
	{
		return s->Equals(CSTR("queued")) || s->Equals(CSTR("in_progress"));
	}
	return false;
}

Bool Net::OpenAIResult::IsQueued() const
{
	NN<Text::JSONBase> jsonObj;
	if (!this->responseJSON.SetTo(jsonObj))
	{
		return false;
	}
	NN<Text::String> s;
	if (jsonObj->GetValueString(CSTR("status")).SetTo(s))
	{
		return s->Equals(CSTR("queued"));
	}
	return false;
}

Net::OpenAIStreamResult::OpenAIStreamResult(NN<Net::HTTPClient> cli) : OpenAIResult(cli->GetRespStatus(), CSTR("")), reader(cli)
{
	this->cli = cli;
	this->lastEvent = nullptr;
	this->deltaType = 0;
}

Net::OpenAIStreamResult::~OpenAIStreamResult()
{
	this->cli.Delete();
	OPTSTR_DEL(this->lastEvent);
}

Bool Net::OpenAIStreamResult::IsStream() const
{
	return true;
}

void Net::OpenAIStreamResult::LockAccess(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->mut);
}

Text::CString Net::OpenAIStreamResult::GetOutputReasoning() const
{
	if (this->deltaType == 1)
	{
		return this->sbDelta.ToCString();
	}
	return this->OpenAIResult::GetOutputReasoning();
}

Text::CStringNN Net::OpenAIStreamResult::GetOutputMessage() const
{
	if (this->deltaType == 2)
	{
		return this->sbDelta.ToCString();
	}
	return this->OpenAIResult::GetOutputMessage();
}

Optional<Text::String> Net::OpenAIStreamResult::NextEvent()
{
/*
	response.created
	response.in_progress
	response.output_item.added
	response.content_part.added
	response.reasoning_text.delta *
	response.reasoning_text.done
	response.content_part.done
	response.output_item.done
	response.output_item.added
	response.content_part.added
	response.output_text.delta *
	response.output_text.done
	response.content_part.done
	response.output_item.done
	response.completed
*/
	NN<Text::String> evt;
	Text::StringBuilderUTF8 sb;
	while (this->reader.ReadLine(sb, 65536))
	{
		if (sb.StartsWith(CSTR("event:")))
		{
			evt = Text::String::New(sb.ToCString().Substring(6).LTrim());
			OPTSTR_DEL(this->lastEvent);
			this->lastEvent = evt;
		}
		if (sb.StartsWith(CSTR("data:")))
		{
			if (this->lastEvent.SetTo(evt))
			{
#if defined(VERBOSE)
				printf("OpenAIStreamResult.NextEvent: event=%s\n", evt->v.Ptr());
#endif
				NN<Text::JSONBase> json;
				NN<Text::JSONBase> oldJson;
				NN<Text::JSONObject> jsonObj;
				NN<Text::JSONArray> jsonArr;
				NN<Text::String> s;
				if (Text::JSONBase::ParseJSONStr(sb.ToCString().Substring(5).LTrim()).SetTo(json))
				{
					if (evt->Equals(CSTR("response.created")))
					{
						// type = response.created
						// response = {}
						// sequence_number = 0
						if (json->GetValueObject(CSTR("response")).SetTo(jsonObj))
						{
							Sync::MutexUsage mutUsage(this->mut);
							if (this->responseJSON.SetTo(oldJson))
							{
								oldJson->EndUse();
							}
							jsonObj->BeginUse();
							this->responseJSON = jsonObj;
						}
					}
					else if (evt->Equals(CSTR("response.in_progress")))
					{
						// type = response.in_progress
						// response = {}
						// sequence_number = 0..n
					}
					else if (evt->Equals(CSTR("response.output_item.added")))
					{
						// type = response.output_item.added
						// output_index = 0..n
						// item = {}
						// sequence_number = 0..n
						if (json->GetValueString(CSTR("item.type")).SetTo(s))
						{
							Sync::MutexUsage mutUsage(this->mut);
							this->sbDelta.ClearStr();
							if (s->Equals(CSTR("reasoning")))
							{
								this->deltaType = 1;
							}
							else if (s->Equals(CSTR("message")))
							{
								this->deltaType = 2;
							}
							else
							{
								this->deltaType = 0;
							}
						}
					}
					else if (evt->Equals(CSTR("response.output_item.done")))
					{
						// type = response.output_item.done
						// output_index = 0..n
						// item = {}
						// sequence_number = 0..n
						if (this->responseJSON.SetTo(oldJson) && oldJson->GetValueArray(CSTR("output")).SetTo(jsonArr) && json->GetValueObject(CSTR("item")).SetTo(jsonObj))
						{
							Sync::MutexUsage mutUsage(this->mut);
							jsonArr->AddArrayValue(jsonObj);
							this->deltaType = 0;
						}
#if defined(VERBOSE)
						else
						{
							printf("OpenAIStreamResult.NextEvent: unexpected json, event=response.output_item.done: json=%s\n", sb.ToCString().Substring(5).LTrim().v.Ptr());
						}
#endif
					}
					else if (evt->Equals(CSTR("response.reasoning_text.delta")))
					{
						// type = response.reasoning_text.delta
						// item_id = "???"
						// output_index = 0..n
						// content_index == 0..n
						// delta = "?"
						// sequence_number = 0..n
						Sync::MutexUsage mutUsage(this->mut);
						if (json->GetValueString(CSTR("delta")).SetTo(s) && this->deltaType == 1)
						{
							Sync::MutexUsage mutUsage(this->mut);
							this->sbDelta.Append(s);
						}
					}
					else if (evt->Equals(CSTR("response.output_text.delta")))
					{
						// type = response.output_text.delta
						// item_id = "???"
						// output_index = 0..n
						// content_index == 0..n
						// delta = "?"
						// logprobs = []
						// sequence_number = 0..n
						if (json->GetValueString(CSTR("delta")).SetTo(s) && this->deltaType == 2)
						{
							Sync::MutexUsage mutUsage(this->mut);
							this->sbDelta.Append(s);
						}
					}
					else if (evt->Equals(CSTR("response.content_part.added")))
					{
						// type = response.content_part.added
						// item_id = "???"
						// output_index = 0..n
						// content_index == 0..n
						// part = {}
						// sequence_number = 0..n
					}
					else if (evt->Equals(CSTR("response.reasoning_text.done")))
					{
						// type = response.reasoning_text.done
						// item_id = "???"
						// output_index = 0..n
						// content_index == 0..n
						// text = "?"
						// sequence_number = 0..n
					}
					else if (evt->Equals(CSTR("response.output_text.done")))
					{
						// type = response.output_text.done
						// item_id = "???"
						// output_index = 0..n
						// content_index == 0..n
						// text = "?"
						// logprobs = []
						// sequence_number = 0..n
					}
					else if (evt->Equals(CSTR("response.content_part.done")))
					{
						// type = response.content_part.done
						// item_id = "???"
						// output_index = 0..n
						// content_index == 0..n
						// part = {}
						// sequence_number = 0..n
					}
					else if (evt->Equals(CSTR("response.completed")))
					{
						// type = response.completed
						// response = {}
						// sequence_number = 0..n
						if (json->GetValueObject(CSTR("response")).SetTo(jsonObj))
						{
							Sync::MutexUsage mutUsage(this->mut);
							if (this->responseJSON.SetTo(oldJson))
							{
								oldJson->EndUse();
							}
							jsonObj->BeginUse();
							this->responseJSON = jsonObj;
						}
					}
					else
					{
#if defined(VERBOSE)
						printf("OpenAIStreamResult.NextEvent: unsupported, event=%s, data=%s\n", evt->v.Ptr(), sb.ToCString().Substring(5).LTrim().v.Ptr());
#endif
					}
					json->EndUse();
				}
				return evt;
			}
			else
			{
#if defined(VERBOSE)
				printf("OpenAIStreamResult.NextEvent: event type not found before data\n");
#endif
				return nullptr;
			}
		}
		sb.ClearStr();
	}
	return nullptr;
}
