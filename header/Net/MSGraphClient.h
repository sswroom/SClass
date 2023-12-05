#ifndef _SM_NET_MSGRAPHCLIENT
#define _SM_NET_MSGRAPHCLIENT
#include "Data/Timestamp.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngine.h"
#include "Net/SocketFactory.h"
#include "Text/JSON.h"
#include "Text/String.h"

namespace Net
{
	class MSGraphAccessToken
	{
	private:
		NotNullPtr<Text::String> type;
		Data::Timestamp expiresIn;
		Data::Timestamp extExpiresIn;
		NotNullPtr<Text::String> accessToken;

	public:
		MSGraphAccessToken(NotNullPtr<Text::String> type, Int32 expiresIn, Int32 extExpiresIn, NotNullPtr<Text::String> accessToken);
		virtual ~MSGraphAccessToken();

		virtual void InitClient(NotNullPtr<Net::HTTPClient> cli);
	};

	class MSGraphEventMessageRequest
	{
	private:
		static Text::CStringNN knownTypes[];
		NotNullPtr<Text::JSONObject> obj;

	public:
		MSGraphEventMessageRequest(NotNullPtr<Text::JSONObject> obj);
		~MSGraphEventMessageRequest();

		static Optional<MSGraphEventMessageRequest> Parse(NotNullPtr<Text::JSONObject> obj);
		static Bool IsKnownType(Text::CStringNN type);
	};

	class MSGraphMailFolder
	{
	private:
		static Text::CStringNN knownTypes[];
		NotNullPtr<Text::JSONObject> obj;

	public:
		MSGraphMailFolder(NotNullPtr<Text::JSONObject> obj);
		~MSGraphMailFolder();

		Optional<Text::String> GetId();
		Optional<Text::String> GetDisplayName();
		Optional<Text::String> GetParentFolderId();
		Int64 GetChildFolderCount();
		Int64 GetUnreadItemCount();
		Int64 GetTotalItemCount();
		Int64 GetSizeInBytes();
		Bool GetIsHidden();

		static Optional<MSGraphMailFolder> Parse(NotNullPtr<Text::JSONObject> obj);
		static Bool IsKnownType(Text::CStringNN type);
	};

	class MSGraphClient
	{
	private:
		typedef Bool (*IsKnownTypeFunc)(Text::CStringNN type);
		NotNullPtr<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;

		Optional<MSGraphAccessToken> AccessTokenParse(Net::WebStatus::StatusCode status, Text::CStringNN content);

		template<class T> Bool GetList(NotNullPtr<MSGraphAccessToken> token, Text::CStringNN url, Text::CStringNN funcName, NotNullPtr<Data::ArrayListNN<T>> dataList);
	public:
		MSGraphClient(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl);
		~MSGraphClient();

		Optional<MSGraphAccessToken> AccessTokenGet(Text::CStringNN tenantId, Text::CStringNN clientId, Text::CStringNN clientSecret, Text::CString scope);
		Bool EntityGetMe(NotNullPtr<MSGraphAccessToken> token);
		Bool MailMessagesGet(NotNullPtr<MSGraphAccessToken> token, Text::CString userName, UOSInt top, UOSInt skip, NotNullPtr<Data::ArrayListNN<MSGraphEventMessageRequest>> msgList, OutParam<Bool> hasNext);
		Bool MailFoldersGet(NotNullPtr<MSGraphAccessToken> token, Text::CString userName, Bool includeHidden, NotNullPtr<Data::ArrayListNN<MSGraphMailFolder>> folderList);

		static Bool HasUnknownTypes(NotNullPtr<Text::JSONObject> obj, IsKnownTypeFunc isKnownType, Text::CStringNN typeName);
	};
}
#endif
