#ifndef _SM_NET_IAMSMARTAPI
#define _SM_NET_IAMSMARTAPI
#include "Crypto/Cert/X509PrivKey.h"
#include "Data/RandomBytesGenerator.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngine.h"
#include "Text/JSON.h"

namespace Net
{
	class IAMSmartAPI
	{
	public:
		struct CEKInfo
		{
			UnsafeArray<UInt8> key;
			UOSInt keyLen;
			Int64 issueAt;
			Int64 expiresAt;
		};

		struct TokenInfo
		{
			NN<Text::String> accessToken;
			NN<Text::String> openID;
			Int64 issueAt;
			Int64 expiresAt;
			Int64 lastModifiedDate;
		};

		struct ProfileInfo
		{
			Optional<Text::String> hkid;
			UTF8Char hkidChk;
			Optional<Text::String> prefix;
			Optional<Text::String> enName;
			Optional<Text::String> chName;
			Optional<Text::String> chNameVerfied;
			UInt32 birthDate;
			UTF8Char gender;
			UTF8Char maritalStatus;
			Optional<Text::String> homeTelNumberICC;
			Optional<Text::String> homeTelNumber;
			Optional<Text::String> officeTelNumberICC;
			Optional<Text::String> officeTelNumber;
			Optional<Text::String> mobileTelNumberICC;
			Optional<Text::String> mobileTelNumber;
			Optional<Text::String> emailAddress;
			Optional<Text::String> residentialAddress;
			Optional<Text::String> postalAddress;
			UTF8Char educationLevel;
		};
	private:
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		NN<Text::String> domain;
		NN<Text::String> clientID;
		NN<Text::String> clientSecret;
		Data::RandomBytesGenerator rand;

		void InitHTTPClient(NN<Net::HTTPClient> cli, Text::CStringNN content);
		Optional<Text::JSONBase> PostEncReq(Text::CStringNN url, NN<CEKInfo> cek, Text::CStringNN jsonMsg);
		Optional<Text::String> ParseAddress(NN<Text::JSONBase> json, Text::CStringNN path);
	public:
		IAMSmartAPI(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN domain, Text::CStringNN clientID, Text::CStringNN clientSecret);
		~IAMSmartAPI();

		static void FreeCEK(NN<CEKInfo> cek);
		Bool GetKey(NN<Crypto::Cert::X509PrivKey> privKey, NN<CEKInfo> cek);
		Bool RevokeKey();
		static void FreeToken(NN<TokenInfo> token);
		Bool GetToken(Text::CStringNN code, NN<CEKInfo> cek, NN<TokenInfo> token);
		static void FreeProfiles(NN<ProfileInfo> profiles);
		Bool GetProfiles(NN<TokenInfo> token, Text::CStringNN eMEFields, Text::CStringNN profileFields, NN<CEKInfo> cek, NN<ProfileInfo> profiles);
	};
}
#endif
