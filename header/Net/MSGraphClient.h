#ifndef _SM_NET_MSGRAPHCLIENT
#define _SM_NET_MSGRAPHCLIENT
#include "Data/Timestamp.h"
#include "IO/LogTool.h"
#include "Net/HTTPClient.h"
#include "Net/JSONResponse.h"
#include "Net/SSLEngine.h"
#include "Net/SocketFactory.h"
#include "Text/JSON.h"
#include "Text/String.h"

namespace Net
{
	class MSGraphAccessToken
	{
	private:
		NN<Text::String> type;
		Data::Timestamp expiresIn;
		Data::Timestamp extExpiresIn;
		NN<Text::String> accessToken;

	public:
		MSGraphAccessToken(NN<Text::String> type, Int32 expiresIn, Int32 extExpiresIn, NN<Text::String> accessToken);
		virtual ~MSGraphAccessToken();

		virtual void InitClient(NN<Net::HTTPClient> cli);
	};

	JSONRESP_BEGIN(MSGraphEntity)
	JSONRESP_SEC_GET(MSGraphEntity)
	JSONRESP_END

	JSONRESP_BEGIN(MSGraphMessageCategory)
	JSONRESP_SEC_GET(MSGraphMessageCategory)
	JSONRESP_END

	JSONRESP_BEGIN(MSGraphDateTime)
	JSONRESP_STR("dateTime",false,false)
	JSONRESP_STR("timeZone",false,false)
	JSONRESP_SEC_GET(MSGraphDateTime)
	JSONRESP_GETSTR("dateTime",GetDateTime)
	JSONRESP_GETSTR("timeZone",GetTimeZone)
	JSONRESP_END

	JSONRESP_BEGIN(MSGraphLocation)
	JSONRESP_STR("displayName",false,false)
	JSONRESP_STR("locationType",false,false)
	JSONRESP_STR("uniqueIdType",false,false)
	JSONRESP_SEC_GET(MSGraphLocation)
	JSONRESP_GETSTR("displayName",GetDisplayName)
	JSONRESP_GETSTR("locationType",GetLocationType)
	JSONRESP_GETSTR("uniqueIdType",GetUniqueIdType)
	JSONRESP_END

	JSONRESP_BEGIN(MSGraphMessageBody)
	JSONRESP_STR("content",false,false)
	JSONRESP_STR("contentType",false,false)
	JSONRESP_SEC_GET(MSGraphMessageBody)
	JSONRESP_GETSTR("content",GetContent)
	JSONRESP_GETSTR("contentType",GetContentType)
	JSONRESP_END

	JSONRESP_BEGIN(MSGraphMessageFlag)
	JSONRESP_STR("flagStatus",false,false)
	JSONRESP_SEC_GET(MSGraphMessageFlag)
	JSONRESP_GETSTR("flagStatus",GetFlagStatus)
	JSONRESP_END

	JSONRESP_BEGIN(MSGraphEmailAddress)
	JSONRESP_STR("address",false,false)
	JSONRESP_STR("name",false,false)
	JSONRESP_SEC_GET(MSGraphEmailAddress)
	JSONRESP_GETSTR("address",GetAddress)
	JSONRESP_GETSTR("name",GetName)
	JSONRESP_END

	JSONRESP_BEGIN(MSGraphMessageReplyTo)
	JSONRESP_OBJ("emailAddress",false,false,MSGraphEmailAddress)
	JSONRESP_SEC_GET(MSGraphMessageReplyTo)
	JSONRESP_GETOBJ("emailAddress",GetEmailAddress,MSGraphEmailAddress)
	JSONRESP_END

	JSONRESP_BEGIN(MSGraphRecipient)
	JSONRESP_OBJ("emailAddress",false,false,MSGraphEmailAddress)
	JSONRESP_SEC_GET(MSGraphRecipient)
	JSONRESP_GETOBJ("emailAddress",GetEmailAddress,MSGraphEmailAddress)
	JSONRESP_END

	JSONRESP_BEGIN(MSGraphEventMessageRequest)
	JSONRESP_STR("@odata.etag",true,false)
	JSONRESP_STR("@odata.type",true,false)
	JSONRESP_OBJ("body",false,false,MSGraphMessageBody)
	JSONRESP_STR("bodyPreview",false,false)
	JSONRESP_STR("changeKey",false,false)
	JSONRESP_STR("conversationId",false,false)
	JSONRESP_STR("conversationIndex",false,false)
	JSONRESP_STR("createdDateTime",false,false)
	JSONRESP_OBJ("flag",false,false,MSGraphMessageFlag)
	JSONRESP_OBJ("from",false,false,MSGraphRecipient)
	JSONRESP_BOOL("hasAttachments",false,false)
	JSONRESP_STR("id",false,false)
	JSONRESP_STR("importance",false,false)
	JSONRESP_STR("inferenceClassification",false,false)
	JSONRESP_STR("internetMessageId",false,false)
	JSONRESP_BOOL("isDeliveryReceiptRequested",false,false)
	JSONRESP_BOOL("isDraft",false,false)
	JSONRESP_BOOL("isRead",false,false)
	JSONRESP_BOOL("isReadReceiptRequested",false,false)
	JSONRESP_STR("lastModifiedDateTime",false,false)
	JSONRESP_STR("parentFolderId",false,false)
	JSONRESP_STR("receivedDateTime",false,false)
	JSONRESP_OBJ("sender",false,false,MSGraphRecipient)
	JSONRESP_STR("sentDateTime",false,false)
	JSONRESP_STR("subject",false,false)
	JSONRESP_ARRAY_OBJ("toRecipients",false,false,MSGraphRecipient)
	JSONRESP_STR("webLink",false,false)
	JSONRESP_ARRAY_OBJ("ccRecipients",false,false,MSGraphRecipient)
	JSONRESP_ARRAY_OBJ("bccRecipients",false,false,MSGraphRecipient)
	JSONRESP_ARRAY_OBJ("categories",false,false,MSGraphMessageCategory)
	JSONRESP_ARRAY_OBJ("replyTo",false,false,MSGraphMessageReplyTo)
	JSONRESP_STR("allowNewTimeProposals",true,true)
	JSONRESP_OBJ("endDateTime",true,false,MSGraphDateTime)
	JSONRESP_BOOL("isAllDay",true,false)
	JSONRESP_BOOL("isDelegated",true,false)
	JSONRESP_BOOL("isOutOfDate",true,false)
	JSONRESP_OBJ("location",true,false,MSGraphLocation)
	JSONRESP_STR("meetingMessageType",true,false)
	JSONRESP_STR("meetingRequestType",true,false)
	JSONRESP_OBJ("previousEndDateTime",true,true,MSGraphDateTime)
	JSONRESP_OBJ("previousLocation",true,true,MSGraphLocation)
	JSONRESP_OBJ("previousStartDateTime",true,true,MSGraphDateTime)
	JSONRESP_STR("recurrence",true,true)
	JSONRESP_BOOL("responseRequested",true,false)
	JSONRESP_OBJ("startDateTime",true,false,MSGraphDateTime)
	JSONRESP_STR("type",true,false)
	JSONRESP_SEC_GET(MSGraphEventMessageRequest)
//	JSONRESP_GETSTR("@odata.etag",Get@odata.etag)
	JSONRESP_GETOBJ("body",GetBody,MSGraphMessageBody)
	JSONRESP_GETSTR("bodyPreview",GetBodyPreview)
	JSONRESP_GETSTR("changeKey",GetChangeKey)
	JSONRESP_GETSTR("conversationId",GetConversationId)
	JSONRESP_GETSTR("conversationIndex",GetConversationIndex)
	JSONRESP_GETSTR("createdDateTime",GetCreatedDateTime)
	JSONRESP_GETOBJ("flag",GetFlag,MSGraphMessageFlag)
	JSONRESP_GETOBJ("from",GetFrom,MSGraphRecipient)
	JSONRESP_GETBOOL("hasAttachments",HasAttachments)
	JSONRESP_GETSTR("id",GetId)
	JSONRESP_GETSTR("importance",GetImportance)
	JSONRESP_GETSTR("inferenceClassification",GetInferenceClassification)
	JSONRESP_GETSTR("internetMessageId",GetInternetMessageId)
	JSONRESP_GETBOOL("isDeliveryReceiptRequested",IsDeliveryReceiptRequested)
	JSONRESP_GETBOOL("isDraft",IsDraft)
	JSONRESP_GETBOOL("isRead",IsRead)
	JSONRESP_GETBOOL("isReadReceiptRequested",IsReadReceiptRequested)
	JSONRESP_GETSTR("lastModifiedDateTime",GetLastModifiedDateTime)
	JSONRESP_GETSTR("parentFolderId",GetParentFolderId)
	JSONRESP_GETSTR("receivedDateTime",GetReceivedDateTime)
	JSONRESP_GETOBJ("sender",GetSender,MSGraphRecipient)
	JSONRESP_GETSTR("sentDateTime",GetSentDateTime)
	JSONRESP_GETSTR("subject",GetSubject)
	JSONRESP_GETARRAY_OBJ("toRecipients",GetToRecipients,MSGraphRecipient)
	JSONRESP_GETSTR("webLink",GetWebLink)
	JSONRESP_GETARRAY_OBJ("ccRecipients",GetCcRecipients,MSGraphRecipient)
	JSONRESP_GETARRAY_OBJ("bccRecipients",GetBccRecipients,MSGraphRecipient)
	JSONRESP_GETARRAY_OBJ("categories",GetCategories,MSGraphMessageCategory)
	JSONRESP_GETARRAY_OBJ("replyTo",GetReplyTo,MSGraphMessageReplyTo)
	JSONRESP_GETSTROPT("allowNewTimeProposals",GetAllowNewTimeProposals)
	JSONRESP_GETOBJ("endDateTime",GetEndDateTime,MSGraphDateTime)
	JSONRESP_GETBOOL("isAllDay",IsAllDay)
	JSONRESP_GETBOOL("isDelegated",IsDelegated)
	JSONRESP_GETBOOL("isOutOfDate",IsOutOfDate)
	JSONRESP_GETOBJ("location",GetLocation,MSGraphLocation)
	JSONRESP_GETSTROPT("meetingMessageType",GetMeetingMessageType)
	JSONRESP_GETSTROPT("meetingRequestType",GetMeetingRequestType)
	JSONRESP_GETOBJ("previousEndDateTime",GetPreviousEndDateTime,MSGraphDateTime)
	JSONRESP_GETOBJ("previousLocation",GetPreviousLocation,MSGraphLocation)
	JSONRESP_GETOBJ("previousStartDateTime",GetPreviousStartDateTime,MSGraphDateTime)
	JSONRESP_GETSTROPT("recurrence",GetRecurrence)
	JSONRESP_GETBOOL("responseRequested",IsResponseRequested)
	JSONRESP_GETOBJ("startDateTime",GetStartDateTime,MSGraphDateTime)
	JSONRESP_GETSTROPT("type",GetType)
	JSONRESP_END

	JSONRESP_BEGIN(MSGraphMailFolder)
	JSONRESP_STR("id", false, false)
	JSONRESP_STR("displayName", false, false)
	JSONRESP_DOUBLE("childFolderCount",false,false)
	JSONRESP_BOOL("isHidden",false,false)
	JSONRESP_STR("parentFolderId",false,false)
	JSONRESP_DOUBLE("sizeInBytes",false,false)
	JSONRESP_DOUBLE("totalItemCount",false,false)
	JSONRESP_DOUBLE("unreadItemCount",false,false)
	JSONRESP_SEC_GET(MSGraphMailFolder)
	JSONRESP_GETSTR("id", GetId)
	JSONRESP_GETSTR("displayName", GetDisplayName)
	JSONRESP_GETDOUBLE("childFolderCount",GetChildFolderCount,0.0)
	JSONRESP_GETBOOL("isHidden",IsHidden)
	JSONRESP_GETSTR("parentFolderId",GetParentFolderId)
	JSONRESP_GETDOUBLE("sizeInBytes",GetSizeInBytes,0.0)
	JSONRESP_GETDOUBLE("totalItemCount",GetTotalItemCount,0.0)
	JSONRESP_GETDOUBLE("unreadItemCount",GetUnreadItemCount,0.0)
	JSONRESP_END

	class MSGraphClient
	{
	private:
		typedef Bool (*IsKnownTypeFunc)(Text::CStringNN type);
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		Optional<IO::LogTool> log;
		Bool debugLog;

		Optional<MSGraphAccessToken> AccessTokenParse(Net::WebStatus::StatusCode status, Text::CStringNN content);

		template<class T> Bool GetList(NN<MSGraphAccessToken> token, Text::CStringNN url, Text::CStringNN funcName, NN<Data::ArrayListNN<T>> dataList);
	public:
		MSGraphClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl);
		~MSGraphClient();

		void SetLog(NN<IO::LogTool> log, Bool debugLog);

		Optional<MSGraphAccessToken> AccessTokenGet(Text::CStringNN tenantId, Text::CStringNN clientId, Text::CStringNN clientSecret, Text::CString scope);
		Optional<MSGraphEntity> EntityGet(NN<MSGraphAccessToken> token, Text::CString userName);
		Bool MailMessagesGet(NN<MSGraphAccessToken> token, Text::CString userName, UOSInt top, UOSInt skip, NN<Data::ArrayListNN<MSGraphEventMessageRequest>> msgList, OutParam<Bool> hasNext);
		Bool MailFoldersGet(NN<MSGraphAccessToken> token, Text::CString userName, Bool includeHidden, NN<Data::ArrayListNN<MSGraphMailFolder>> folderList);

		static Bool HasUnknownTypes(NN<Text::JSONObject> obj, IsKnownTypeFunc isKnownType, Text::CStringNN typeName);
	};
}
#endif
