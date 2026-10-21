#ifndef _SM_NET_OPENAICLIENT
#define _SM_NET_OPENAICLIENT
#include "Net/HTTPClient.h"
#include "Net/SSLEngine.h"
#include "Sync/MutexUsage.h"
#include "Text/JSON.h"
#include "Text/UTF8Reader.h"

namespace Net
{
	class OpenAIResult;
	class OpenAIChatCompletion;
	class OpenAIResponse;
	enum class OpenAIReasoningEffort
	{
		Default,
		None,
		Low,
		Medium,
		High
	};

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
		NN<OpenAIResult> GetResponseResult(Text::CStringNN responseId);
	};

	enum class OpenAIMessageRole
	{
		System,
		User,
		Assistant,
		Tool
	};

	class OpenAIMessage
	{
	private:
		Optional<OpenAIMessage> next;
		OpenAIMessageRole role;
		NN<Text::String> content;
		Optional<Text::String> rawMessage;
		Optional<Text::String> toolCallId;

		OpenAIMessage(NN<OpenAIMessage> msg);
	public:
		OpenAIMessage(OpenAIMessageRole role, Text::CStringNN content);
		OpenAIMessage(Text::CStringNN rawMessage);
		OpenAIMessage(Text::CStringNN toolCallId, Text::CStringNN content);
		~OpenAIMessage();

		NN<OpenAIMessage> Clone() const;
		OpenAIMessageRole GetRole() const;
		Text::CStringNN GetContent() const;
		Optional<OpenAIMessage> GetNext() const;
		void SetNext(Optional<OpenAIMessage> next);
		void ToJSON(NN<Text::StringBuilderUTF8> sb) const;
	};

	enum class OpenAIToolParameterType
	{
		String,
		Number,
		Integer,
		Boolean
	};

	class OpenAIToolParameters
	{
	private:
		class Parameter
		{
		public:
			NN<Text::String> name;
			NN<Text::String> description;
			OpenAIToolParameterType type;
			Bool required;

			Parameter(Text::CStringNN name, Text::CStringNN description, OpenAIToolParameterType type, Bool required);
			~Parameter();
		};

		IntOS useCnt;
		Data::ArrayListNN<Parameter> parameterList;

	public:
		OpenAIToolParameters();
		~OpenAIToolParameters();

		void BeginUse();
		void EndUse();
		void AddParameter(Text::CStringNN name, Text::CStringNN description, OpenAIToolParameterType type, Bool required);
		void ToJSON(NN<Text::StringBuilderUTF8> sb) const;
	};

	class OpenAITool
	{
	private:
		IntOS useCnt;
		NN<Text::String> name;
		NN<Text::String> description;
		NN<OpenAIToolParameters> parameters;
		Bool strict;

	public:
		OpenAITool(Text::CStringNN name, Text::CStringNN description, NN<OpenAIToolParameters> parameters);
		~OpenAITool();

		void BeginUse();
		void EndUse();
		void SetStrict(Bool strict);
		void ToJSON(NN<Text::StringBuilderUTF8> sb, Bool chatCompletion) const;
	};

	class OpenAIChatCompletion
	{
	private:
		Text::CStringNN model;
		Optional<OpenAIMessage> firstMessage;
		Optional<OpenAIMessage> lastMessage;
		Data::ArrayListNN<OpenAITool> tools;

		void AddMessage(NN<OpenAIMessage> msg);
	public:
		OpenAIChatCompletion(Text::CStringNN model);
		~OpenAIChatCompletion();

		Text::CStringNN GetModel() const;
		Optional<OpenAIMessage> GetFirstMessage() const;
		void AddMessage(OpenAIMessageRole role, Text::CStringNN content);
		void AddAssistantToolCallMessage(Text::CStringNN rawMessage);
		void AddToolResultMessage(Text::CStringNN toolCallId, Text::CStringNN content);
		void AddTool(NN<OpenAITool> tool);
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
		OpenAIReasoningEffort reasoningEffort;
		UIntOS maxTokens;
		Double temperature;
		Double topP;
		Bool background;
		Bool stream;
		Data::ArrayListNN<OpenAITool> tools;

	public:
		OpenAIResponse(Text::CStringNN model, Text::CStringNN userInput, Text::CString systemInput);
		~OpenAIResponse();

		Text::CStringNN GetModel() const;
		Text::CStringNN GetUserInput() const;
		Text::CString GetSystemInput() const;
		void SetPreviousResponseId(Text::CStringNN responseId);
		void SetReasoningEffort(OpenAIReasoningEffort effort);
		void SetMaxTokens(UIntOS maxTokens);
		void SetTemperature(Double temperature);
		void SetTopP(Double topP);
		void SetBackground(Bool background);
		void SetStream(Bool stream);
		void AddTool(NN<OpenAITool> tool);
		Bool AddFile(Text::CStringNN filePath);
		void ToJSON(NN<Text::StringBuilderUTF8> sb) const;
	};

	class OpenAIResult
	{
	protected:
		UInt32 statusCode;
		NN<Text::String> responseText;
		Optional<Text::JSONBase> responseJSON;

		Optional<Text::String> GetJSONStr(Text::CStringNN name) const;
		Bool GetJSONInt64(Text::CStringNN name, OutParam<Int64> value) const;
		Bool GetJSONBool(Text::CStringNN name) const;
		Double GetJSONDoubleOrNAN(Text::CStringNN name) const;
	public:
		OpenAIResult(UInt32 statusCode, Text::CStringNN responseText);
		virtual ~OpenAIResult();

		virtual Optional<Text::String> GetID() const;
		virtual Optional<Text::String> GetObject() const;
		virtual Optional<Text::String> GetStatus() const;
		virtual Optional<Text::String> GetModel() const;
		virtual Bool GetCreatedAt(OutParam<Int64> createdAt) const;
		virtual Bool GetCompletedAt(OutParam<Int64> completedAt) const;
		virtual Optional<Text::String> GetPreviousResponseId() const;
		virtual Optional<Text::String> GetInstructions() const;
		virtual Optional<Text::String> GetError() const;
		virtual Optional<Text::String> GetToolChoice() const;
		virtual Optional<Text::String> GetTruncation() const;
		virtual Bool IsParallelToolCalls() const;
		virtual Double GetTopP() const;
		virtual Double GetPresencePenalty() const;
		virtual Double GetFrequencyPenalty() const;
		virtual Double GetTopLogprobs() const;
		virtual Double GetTemperature() const;
		virtual Bool GetInputTokens(OutParam<Int64> inputTokens) const;
		virtual Bool GetOutputTokens(OutParam<Int64> outputTokens) const;
		virtual Bool GetTotalTokens(OutParam<Int64> totalTokens) const;
		virtual Bool GetInputCachedTokens(OutParam<Int64> inputCachedTokens) const;
		virtual Bool GetOutputReasoningTokens(OutParam<Int64> outputReasoningTokens) const;
		virtual Bool IsStore() const;
		virtual Bool IsBackground() const;
		virtual Bool IsStream() const;
		virtual void LockAccess(NN<Sync::MutexUsage> mutUsage);
		virtual Optional<Text::String> GetServiceTier() const;
		virtual UInt32 GetStatusCode() const;
		NN<Text::String> GetResponseText() const;
		virtual Text::CString GetOutputReasoning() const;
		virtual Text::CStringNN GetOutputMessage() const;
		Bool IsQueuedOrInProgress() const;
		Bool IsQueued() const;
	};
	class OpenAIStreamResult : public OpenAIResult
	{
	private:
		NN<Net::HTTPClient> cli;
		Text::UTF8Reader reader;
		Sync::Mutex mut;
		Optional<Text::String> lastEvent;
		Text::StringBuilderUTF8 sbDelta;
		UIntOS deltaType;

	public:
		OpenAIStreamResult(NN<Net::HTTPClient> cli);
		virtual ~OpenAIStreamResult();

		virtual Bool IsStream() const;
		virtual void LockAccess(NN<Sync::MutexUsage> mutUsage);
		virtual Text::CString GetOutputReasoning() const;
		virtual Text::CStringNN GetOutputMessage() const;
		Optional<Text::String> NextEvent();
	};
}
#endif
