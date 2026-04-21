#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/HTTPClient.h"
#include "Net/MIME.h"
#include "Net/OpenAIClient.h"
#include "Text/JSON.h"
#include "Text/JSText.h"
#include "Text/TextBinEnc/Base64Enc.h"

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

Net::OpenAIResponse::OpenAIResponse(Text::CStringNN model, Text::CStringNN input)
{
	this->model = model;
	this->input = input;
	this->previousResponseId = nullptr;
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

Text::CStringNN Net::OpenAIResponse::GetInput() const
{
	return this->input;
}

void Net::OpenAIResponse::SetPreviousResponseId(Text::CStringNN responseId)
{
	this->previousResponseId = responseId;
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
	sb->Append(CSTR("{\"model\":"));
	Text::JSText::ToJSTextDQuote(sb, this->model.v);
	sb->Append(CSTR(",\"input\":"));
	if (this->fileList.GetCount() > 0)
	{
		sb->AppendUTF8Char('[');
		sb->Append(CSTR("{\"role\":\"user\",\"content\":[{\"type\":\"text\",\"text\":"));
		Text::JSText::ToJSTextDQuote(sb, this->input.v);
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
		Text::JSText::ToJSTextDQuote(sb, this->input.v);
	}
	Text::CStringNN prevId;
	if (this->previousResponseId.SetTo(prevId))
	{
		sb->Append(CSTR(",\"previous_response_id\":"));
		Text::JSText::ToJSTextDQuote(sb, prevId.v);
	}
	sb->AppendUTF8Char('}');
}

Net::OpenAIResult::OpenAIResult(UInt32 statusCode, Text::CStringNN responseText)
{
	this->statusCode = statusCode;
	this->responseText = Text::String::New(responseText);
}

Net::OpenAIResult::~OpenAIResult()
{
	this->responseText->Release();
}

UInt32 Net::OpenAIResult::GetStatusCode() const
{
	return this->statusCode;
}

NN<Text::String> Net::OpenAIResult::GetResponseText() const
{
	return this->responseText;
}

NN<Text::String> Net::OpenAIResult::GetOutputText() const
{
	NN<Text::JSONBase> jsonObj;
	if (!Text::JSONBase::ParseJSONStr(this->responseText->ToCString()).SetTo(jsonObj))
	{
		return this->responseText->Clone();
	}
	NN<Text::String> s;
	if (jsonObj->GetValueNewString(CSTR("output[0].content[0].text")).SetTo(s))
	{
		jsonObj->EndUse();
		return s;
	}
	jsonObj->EndUse();
	return this->responseText->Clone();
}

