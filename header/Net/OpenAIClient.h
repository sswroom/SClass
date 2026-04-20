#ifndef _SM_NET_OPENAICLIENT
#define _SM_NET_OPENAICLIENT
#include "Net/SSLEngine.h"

namespace Net
{
	class OpenAIResult;
	class OpenAIChatCompletion;
	class OpenAIResponse;
	class OpenAIClient
	{
	private:
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		NN<Text::String> apiURL;
		NN<Text::String> apiKey;
		Data::ArrayListStringNN modelList;
		UIntOS modelIndex;
	public:
		OpenAIClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN apiURL, Text::CStringNN apiKey);
		~OpenAIClient();

		Bool IsError() const;
		Bool SetModel(Text::CStringNN model);
		UIntOS GetModelCount() const;
		Optional<Text::String> GetModelName(UIntOS index) const;
		Text::CStringNN GetCurrModel() const;
		NN<OpenAIResult> SendChatCompletion(NN<OpenAIChatCompletion> chat);
		NN<OpenAIResult> SendResponses(NN<OpenAIResponse> resp);
	};

	enum class OpenAIMessageRole
	{
		System,
		User,
		Assistant
	};

	class OpenAIMessage
	{
	private:
		Optional<OpenAIMessage> next;
		OpenAIMessageRole role;
		NN<Text::String> content;

		OpenAIMessage(NN<OpenAIMessage> msg);
	public:
		OpenAIMessage(OpenAIMessageRole role, Text::CStringNN content);
		~OpenAIMessage();

		NN<OpenAIMessage> Clone() const;
		OpenAIMessageRole GetRole() const;
		Text::CStringNN GetContent() const;
		Optional<OpenAIMessage> GetNext() const;
		void SetNext(Optional<OpenAIMessage> next);
		void ToJSON(NN<Text::StringBuilderUTF8> sb) const;
	};

	class OpenAIChatCompletion
	{
	private:
		Text::CStringNN model;
		Optional<OpenAIMessage> firstMessage;
		Optional<OpenAIMessage> lastMessage;
	public:
		OpenAIChatCompletion(Text::CStringNN model);
		~OpenAIChatCompletion();

		Text::CStringNN GetModel() const;
		Optional<OpenAIMessage> GetFirstMessage() const;
		void AddMessage(OpenAIMessageRole role, Text::CStringNN content);
		void ToJSON(NN<Text::StringBuilderUTF8> sb) const;
	};

	class OpenAIResponse
	{
	private:
		Text::CStringNN model;
		Text::CString previousResponseId;
		Text::CStringNN input;
		Data::ArrayListStringNN fileList;
		Data::ArrayListStringNN fileNameList;

	public:
		OpenAIResponse(Text::CStringNN model, Text::CStringNN input);
		~OpenAIResponse();

		Text::CStringNN GetModel() const;
		Text::CStringNN GetInput() const;
		void SetPreviousResponseId(Text::CStringNN responseId);
		Bool AddFile(Text::CStringNN filePath);
		void ToJSON(NN<Text::StringBuilderUTF8> sb) const;
	};

	class OpenAIResult
	{
	private:
		UInt32 statusCode;
		NN<Text::String> responseText;
	public:
		OpenAIResult(UInt32 statusCode, Text::CStringNN responseText);
		~OpenAIResult();

		UInt32 GetStatusCode() const;
		NN<Text::String> GetResponseText() const;
		NN<Text::String> GetOutputText() const;
	};
}
#endif
