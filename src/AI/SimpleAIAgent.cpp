#include "Stdafx.h"
#include "AI/SimpleAIAgent.h"
#include "Data/ArrayListStringNN.h"

//#define VERBOSE

UIntOS AI::SimpleAIAgent::GetUTF8PrefixLength(UnsafeArray<const UTF8Char> text, UIntOS maxLen)
{
	UIntOS prefixLen = maxLen;
	while (prefixLen > 0 && (text[prefixLen] & 0xc0) == 0x80)
	{
		prefixLen--;
	}
	if (prefixLen < maxLen)
	{
		UInt8 c = text[prefixLen];
		UIntOS charLen = c < 0x80 ? 1 : ((c & 0xe0) == 0xc0 ? 2 : ((c & 0xf0) == 0xe0 ? 3 : 4));
		if (prefixLen + charLen > maxLen)
			return prefixLen;
	}
	return maxLen;
}

Bool AI::SimpleAIAgent::SetArgs(NN<Data::VariObject> input, NN<Text::JSONObject> args)
{
	Data::ArrayListStringNN names;
	args->GetObjectNames(names);
	UIntOS i = 0;
	UIntOS j = names.GetCount();
	while (i < j)
	{
		NN<Text::String> name = names.GetItemNoCheck(i);
		NN<Text::JSONBase> value;
		if (!args->GetObjectValue(name->ToCString()).SetTo(value))
		{
			names.Clear();
			return false;
		}

		switch (value->GetType())
		{
		case Text::JSONType::String:
		{
			NN<Text::String> s;
			if (!args->GetObjectString(name->ToCString()).SetTo(s))
			{
				names.Clear();
				return false;
			}
			input->SetItemStr(name->v, s);
			break;
		}
		case Text::JSONType::INT32:
			input->SetItemI64(name->v, value->GetAsInt64());
			break;
		case Text::JSONType::INT64:
			input->SetItemI64(name->v, value->GetAsInt64());
			break;
		case Text::JSONType::Number:
			input->SetItemF64(name->v, value->GetAsDoubleOrNAN());
			break;
		case Text::JSONType::BOOL:
			input->SetItemBool(name->v, value->GetAsBool());
			break;
		case Text::JSONType::Null:
			input->SetItemNull(name->v);
			break;
		case Text::JSONType::Object:
		case Text::JSONType::Array:
		{
			Text::StringBuilderUTF8 sb;
			value->ToJSONString(sb);
			NN<Text::String> valueStr = Text::String::New(sb.ToCString());
			input->SetItemStr(name->v, valueStr);
			valueStr->Release();
			break;
		}
		default:
			names.Clear();
			return false;
		}
		i++;
	}
	names.Clear();
	return true;
}

Optional<Text::String> AI::SimpleAIAgent::SendQuestion(NN<AISession> session, Text::CStringNN question)
{
	NN<Net::OpenAIChatCompletion> chat = session->chat;
	chat->AddMessage(Net::OpenAIMessageRole::User, question);
	UIntOS toolRound = 0;
	while (toolRound < this->maxToolCalls)
	{
		NN<Net::OpenAIResult> result = this->openAI->SendChatCompletion(chat);
		if (result->GetStatusCode() < 200 || result->GetStatusCode() >= 300)
		{
			printf("SimpleAIAgent: HTTP status %u: %s\n", result->GetStatusCode(), result->GetResponseText()->v.Ptr());
			result.Delete();
			return nullptr;
		}

		NN<Text::JSONBase> response;
		if (!Text::JSONBase::ParseJSONStr(result->GetResponseText()->ToCString()).SetTo(response))
		{
			printf("SimpleAIAgent: invalid JSON response: %s\n", result->GetResponseText()->v.Ptr());
			result.Delete();
			return nullptr;
		}
		NN<Text::JSONArray> choices;
		NN<Text::JSONObject> choice;
		NN<Text::JSONObject> message;
		if (!response->GetValueArray(CSTR("choices")).SetTo(choices) ||
			!choices->GetArrayObject(0).SetTo(choice) ||
			!choice->GetObjectObject(CSTR("message")).SetTo(message))
		{
			printf("SimpleAIAgent: response has no choices[0].message: %s\n", result->GetResponseText()->v.Ptr());
			response->EndUse();
			result.Delete();
			return nullptr;
		}

		NN<Text::JSONArray> toolCalls;
		if (!message->GetObjectArray(CSTR("tool_calls")).SetTo(toolCalls) || toolCalls->GetArrayLength() == 0)
		{
			NN<Text::String> content;
			Optional<Text::String> answer = message->GetValueNewString(CSTR("content"));
			if (answer.SetTo(content))
			{
				chat->AddMessage(Net::OpenAIMessageRole::Assistant, content->ToCString());
				session->AddTurn(question, content->ToCString());
				response->EndUse();
				result.Delete();
				return content;
			}
			printf("SimpleAIAgent: assistant response has neither tool calls nor string content: %s\n", result->GetResponseText()->v.Ptr());
			response->EndUse();
			result.Delete();
			return nullptr;
		}

		Text::StringBuilderUTF8 rawMessage;
		message->ToJSONString(rawMessage);
		chat->AddAssistantToolCallMessage(rawMessage.ToCString());

		UIntOS callIndex = 0;
		UIntOS callCount = toolCalls->GetArrayLength();
		while (callIndex < callCount)
		{
			NN<Text::JSONObject> call;
			NN<Text::JSONObject> function;
			NN<Text::String> callId;
			NN<Text::String> functionName;
			NN<Text::String> arguments;
			if (!toolCalls->GetArrayObject(callIndex).SetTo(call) ||
				!call->GetObjectString(CSTR("id")).SetTo(callId) ||
				!call->GetObjectObject(CSTR("function")).SetTo(function) ||
				!function->GetObjectString(CSTR("name")).SetTo(functionName) ||
				!function->GetObjectString(CSTR("arguments")).SetTo(arguments))
			{
				printf("SimpleAIAgent: malformed tool call: %s\n", result->GetResponseText()->v.Ptr());
				response->EndUse();
				result.Delete();
				return nullptr;
			}

			NN<Text::JSONBase> argJSON;
			NN<Text::JSONObject> argObject;
			if (!Text::JSONBase::ParseJSONStr(arguments->ToCString()).SetTo(argJSON))
			{
				printf("SimpleAIAgent: invalid tool arguments: %s\n", arguments->v.Ptr());
				response->EndUse();
				result.Delete();
				return nullptr;
			}
			if (argJSON->GetType() != Text::JSONType::Object)
			{
				printf("SimpleAIAgent: tool arguments are not a JSON object: %s\n", arguments->v.Ptr());
				argJSON->EndUse();
				response->EndUse();
				result.Delete();
				return nullptr;
			}
			argObject = NN<Text::JSONObject>::ConvertFrom(argJSON);
			Data::VariObject input(Data::VariObject::NameType::Field);
			Data::VariObject output(Data::VariObject::NameType::Field);
			Bool invoked = SimpleAIAgent::SetArgs(input, argObject) &&
				this->InvokeSkill(functionName->ToCString(), input, output);
			Text::StringBuilderUTF8 toolOutput;
			if (!invoked)
			{
				toolOutput.Append(CSTR("Tool invocation failed."));
			}
			else
			{
				NN<Data::VariItem> dataItem;
				NN<Text::String> data;
				if (output.GetItem(U8STR("data")).SetTo(dataItem) && dataItem->GetAsNewString().SetTo(data))
				{
					toolOutput.Append(data);
					data->Release();
				}
				else
				{
					output.ToString(toolOutput);
				}
			}
			if (toolOutput.GetLength() > 8192)
			{
				Text::CStringNN toolOutputStr = toolOutput.ToCString();
				UIntOS prefixLen = SimpleAIAgent::GetUTF8PrefixLength(toolOutputStr.v, 8192);
				Text::StringBuilderUTF8 cappedOutput;
				cappedOutput.AppendC(toolOutputStr.v, prefixLen);
				cappedOutput.Append(CSTR("\n[Tool result truncated at 8192 bytes.]"));
				chat->AddToolResultMessage(callId->ToCString(), cappedOutput.ToCString());
			}
			else
			{
				chat->AddToolResultMessage(callId->ToCString(), toolOutput.ToCString());
			}
			argJSON->EndUse();
			callIndex++;
		}
		response->EndUse();
		result.Delete();
		toolRound++;
	}
	printf("SimpleAIAgent: stopped after %d tool-call rounds without a final answer\n", (Int32)this->maxToolCalls);
	return nullptr;
}

AI::SimpleAIAgent::SimpleAIAgent(NN<Net::OpenAIClient> openAI, Text::CStringNN modelName) : openAI(openAI), modelName(Text::String::New(modelName))
{
	this->maxToolCalls = 8;
}

AI::SimpleAIAgent::~SimpleAIAgent()
{
	this->modelName->Release();
}

AI::AISession::AISession(Text::CStringNN modelName, Text::CStringNN systemPrompt) : modelName(Text::String::New(modelName))
{
	NEW_CLASSNN(this->chat, Net::OpenAIChatCompletion(this->modelName->ToCString()));
	this->chat->AddMessage(Net::OpenAIMessageRole::System, systemPrompt);
}

AI::AISession::~AISession()
{
	this->chat.Delete();
	this->sessionLines.FreeAll();
	this->modelName->Release();
}

NN<Text::String> AI::AISession::GetText() const
{
	Text::StringBuilderUTF8 sb;
	UIntOS i = 0;
	UIntOS count = this->sessionLines.GetCount();
	while (i < count)
	{
		sb.Append(this->sessionLines.GetItemNoCheck(i));
		i++;
	}
	return Text::String::New(sb.ToCString());
}

UIntOS AI::AISession::GetMessageCount() const
{
	UIntOS count = 0;
	NN<Net::OpenAIMessage> message;
	if (this->chat->GetFirstMessage().SetTo(message))
	{
		do
		{
			count++;
		}
		while (message->GetNext().SetTo(message));
	}
	return count;
}

Optional<Net::OpenAIMessage> AI::AISession::GetMessage(UIntOS index) const
{
	NN<Net::OpenAIMessage> message;
	if (!this->chat->GetFirstMessage().SetTo(message))
		return nullptr;
	while (index > 0)
	{
		if (!message->GetNext().SetTo(message))
			return nullptr;
		index--;
	}
	return message;
}

void AI::AISession::AddTurn(Text::CStringNN question, Text::CStringNN answer)
{
	Text::StringBuilderUTF8 userLine;
	userLine.Append(CSTR("User: "));
	userLine.Append(question);
	userLine.AppendUTF8Char('\n');
	this->sessionLines.Add(Text::String::New(userLine.ToCString()));
	Text::StringBuilderUTF8 assistantLine;
	assistantLine.Append(CSTR("Assistant: "));
	assistantLine.Append(answer);
	assistantLine.AppendUTF8Char('\n');
	this->sessionLines.Add(Text::String::New(assistantLine.ToCString()));
}

NN<AI::AISession> AI::SimpleAIAgent::CreateSession(Text::CStringNN systemPrompt) const
{
	NN<AISession> session;
	NEW_CLASSNN(session, AISession(this->modelName->ToCString(), systemPrompt));

	UIntOS i = 0;
	UIntOS skillCount = this->GetSkillCount();
	while (i < skillCount)
	{
		NN<AI::AISkill> skill;
		if (this->GetSkill(i).SetTo(skill))
		{
			NN<Net::OpenAIToolParameters> parameters;
			NEW_CLASSNN(parameters, Net::OpenAIToolParameters());
			skill->GetParameters(parameters);
			Text::StringBuilderUTF8 description;
			skill->GetDescription(description);
			NN<Net::OpenAITool> tool;
			NEW_CLASSNN(tool, Net::OpenAITool(skill->GetName(), description.ToCString(), parameters));
			session->chat->AddTool(tool);
			tool->EndUse();
			parameters->EndUse();
		}
		i++;
	}
	return session;
}

Optional<Text::String> AI::SimpleAIAgent::Ask(NN<AISession> session, Text::CStringNN question)
{
	return this->SendQuestion(session, question);
}

void AI::SimpleAIAgent::SetMaxToolCalls(UIntOS maxToolCalls)
{
	this->maxToolCalls = maxToolCalls;
}