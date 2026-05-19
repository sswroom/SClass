#ifndef _SM_NET_OPENAICLIENT
#define _SM_NET_OPENAICLIENT
#include "Net/SSLEngine.h"
#include "Text/JSON.h"

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
		Text::CStringNN userInput;
		Text::CString systemInput;
		Data::ArrayListStringNN fileList;
		Data::ArrayListStringNN fileNameList;

	public:
		OpenAIResponse(Text::CStringNN model, Text::CStringNN userInput, Text::CString systemInput);
		~OpenAIResponse();

		Text::CStringNN GetModel() const;
		Text::CStringNN GetUserInput() const;
		Text::CString GetSystemInput() const;
		void SetPreviousResponseId(Text::CStringNN responseId);
		Bool AddFile(Text::CStringNN filePath);
		void ToJSON(NN<Text::StringBuilderUTF8> sb) const;
	};

	class OpenAIResult
	{
	private:
		UInt32 statusCode;
		NN<Text::String> responseText;
		Optional<Text::JSONBase> responseJSON;

		Optional<Text::String> GetJSONStr(Text::CStringNN name) const;
		Bool GetJSONInt64(Text::CStringNN name, OutParam<Int64> value) const;
		Bool GetJSONBool(Text::CStringNN name) const;
		Double GetJSONDoubleOrNAN(Text::CStringNN name) const;
	public:
		OpenAIResult(UInt32 statusCode, Text::CStringNN responseText);
		~OpenAIResult();

		Optional<Text::String> GetID() const;
		Optional<Text::String> GetObject() const;
		Optional<Text::String> GetStatus() const;
		Optional<Text::String> GetModel() const;
		Bool GetCreatedAt(OutParam<Int64> createdAt) const;
		Bool GetCompletedAt(OutParam<Int64> completedAt) const;
		Optional<Text::String> GetPreviousResponseId() const;
		Optional<Text::String> GetInstructions() const;
		Optional<Text::String> GetError() const;
		Optional<Text::String> GetToolChoice() const;
		Optional<Text::String> GetTruncation() const;
		Bool IsParallelToolCalls() const;
		Double GetTopP() const;
		Double GetPresencePenalty() const;
		Double GetFrequencyPenalty() const;
		Double GetTopLogprobs() const;
		Double GetTemperature() const;
		Bool GetInputTokens(OutParam<Int64> inputTokens) const;
		Bool GetOutputTokens(OutParam<Int64> outputTokens) const;
		Bool GetTotalTokens(OutParam<Int64> totalTokens) const;
		Bool GetInputCachedTokens(OutParam<Int64> inputCachedTokens) const;
		Bool GetOutputReasoningTokens(OutParam<Int64> outputReasoningTokens) const;
		Bool IsStore() const;
		Bool IsBackground() const;
		Optional<Text::String> GetServiceTier() const;
		UInt32 GetStatusCode() const;
		NN<Text::String> GetResponseText() const;
		Optional<Text::String> GetOutputReasoning() const;
		NN<Text::String> GetOutputMessage() const;
	};
}
#endif
