#ifndef _SM_NET_SAMLSSORESPONSE
#define _SM_NET_SAMLSSORESPONSE
#include "Data/Timestamp.h"
#include "Net/SAMLStatusCode.h"
#include "Text/String.h"

namespace Net
{
	class SAMLSSOResponse
	{
	public:
		enum class ResponseError
		{
			Success,
			ResponseNotFound,
			IDPMissing,
			SignKeyMissing,
			SignKeyInvalid,
			ResponseFormatError,
			DecryptFailed,
			UnexpectedIssuer,
			TimeOutOfRange,
			InvalidAudience,
			UsernameMissing
		};
	private:
		ResponseError error;
		NN<Text::String> errorMessage;
		SAMLStatusCode statusCode;
		Optional<Text::String> rawResponse;
		Optional<Text::String> decResponse;
		Optional<Text::String> id;
		Optional<Text::String> sessionIndex;
		Optional<Text::String> issuer;
		Optional<Text::String> audience;
		Data::Timestamp notBefore;
		Data::Timestamp notOnOrAfter;
		Optional<Text::String> nameID;
		Optional<Text::String> name;
		Optional<Text::String> givenname;
		Optional<Text::String> surname;
		Optional<Text::String> emailAddress;

	public:
		SAMLSSOResponse(ResponseError error, Text::CStringNN errorMessage);
		~SAMLSSOResponse();

		ResponseError GetError() const { return this->error; }
		void SetError(ResponseError error) { this->error = error; }
		NN<Text::String> GetErrorMessage() const { return this->errorMessage; }
		void SetErrorMessage(Text::CStringNN errorMessage) { this->errorMessage->Release(); this->errorMessage = Text::String::New(errorMessage); }
		SAMLStatusCode GetStatusCode() const { return this->statusCode; }
		void SetStatusCode(SAMLStatusCode statusCode) { this->statusCode = statusCode; }
		Optional<Text::String> GetRawResponse() const { return this->rawResponse; }
		void SetRawResponse(Text::CString rawResponse) { OPTSTR_DEL(this->rawResponse); this->rawResponse = Text::String::NewOrNull(rawResponse); }
		Optional<Text::String> GetDecResponse() const { return this->decResponse; }
		void SetDecResponse(Text::CString decResponse) { OPTSTR_DEL(this->decResponse); this->decResponse = Text::String::NewOrNull(decResponse); }
		Optional<Text::String> GetId() const { return this->id; }
		void SetId(Text::CString id) { OPTSTR_DEL(this->id); this->id = Text::String::NewOrNull(id); }
		Optional<Text::String> GetSessionIndex() const { return this->sessionIndex; }
		void SetSessionIndex(Text::CString sessionIndex) { OPTSTR_DEL(this->sessionIndex); this->sessionIndex = Text::String::NewOrNull(sessionIndex); }
		Optional<Text::String> GetIssuer() const { return this->issuer; }
		void SetIssuer(Text::CString issuer) { OPTSTR_DEL(this->issuer); this->issuer = Text::String::NewOrNull(issuer); }
		Optional<Text::String> GetAudience() const { return this->audience; }
		void SetAudience(Text::CString audience) { OPTSTR_DEL(this->audience); this->audience = Text::String::NewOrNull(audience); }
		Data::Timestamp GetNotBefore() const { return this->notBefore; }
		void SetNotBefore(Data::Timestamp notBefore) { this->notBefore = notBefore; }
		Data::Timestamp GetNotOnOrAfter() const { return this->notOnOrAfter; }
		void SetNotOnOrAfter(Data::Timestamp notOnOrAfter) { this->notOnOrAfter = notOnOrAfter; }
		Optional<Text::String> GetNameID() const { return this->nameID; }
		void SetNameID(Text::CString nameID) { OPTSTR_DEL(this->nameID); this->nameID = Text::String::NewOrNull(nameID); }
		Optional<Text::String> GetName() const { return this->name; }
		void SetName(Text::CString name) { OPTSTR_DEL(this->name); this->name = Text::String::NewOrNull(name); }
		Optional<Text::String> GetGivenname() const { return this->givenname; }
		void SetGivenname(Text::CString givenname) { OPTSTR_DEL(this->givenname); this->givenname = Text::String::NewOrNull(givenname); }
		Optional<Text::String> GetSurname() const { return this->surname; }
		void SetSurname(Text::CString surname) { OPTSTR_DEL(this->surname); this->surname = Text::String::NewOrNull(surname); }
		Optional<Text::String> GetEmailAddress() const { return this->emailAddress; }
		void SetEmailAddress(Text::CString emailAddress) { OPTSTR_DEL(this->emailAddress); this->emailAddress = Text::String::NewOrNull(emailAddress); }

		static Text::CStringNN ResponseErrorGetName(Net::SAMLSSOResponse::ResponseError val);
	};
}
#endif
