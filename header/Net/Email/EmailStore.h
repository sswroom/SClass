#ifndef _SM_NET_EMAIL_EMAILSTORE
#define _SM_NET_EMAIL_EMAILSTORE
#include "Data/StringUTF8Map.hpp"
#include "IO/StreamData.h"
#include "Net/Email/MailController.h"
#include "Net/Email/SMTPServer.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/MIMEObj/MailMessage.h"

namespace Net
{
	namespace Email
	{
		class EmailStore
		{
		public:
			struct EmailInfo
			{
				Int64 id;
				Net::SocketUtil::AddressInfo remoteAddr;
				Int64 recvTime;
				NN<Text::String> fromAddr;
				UIntOS fileSize;
				Bool isDeleted;
			};

			struct MessageStat
			{
				UIntOS unreadCount;
				UInt64 unreadSize;
			};
		protected:
			Data::ArrayListNN<EmailInfo> mailList;
			Sync::Mutex mailMut;

		public:
			EmailStore();
			virtual ~EmailStore();

			UIntOS GetAllEmails(NN<Data::ArrayListNN<EmailInfo>> emailList);

			virtual Int64 NextEmailId() = 0;
			virtual Bool NewEmail(Int64 id, NN<const Net::SocketUtil::AddressInfo> remoteAddr, Text::CStringNN serverName, NN<const Net::Email::SMTPServer::MailStatus> mail) = 0;
			virtual Bool NewEmail(Int64 id, NN<const Net::SocketUtil::AddressInfo> remoteAddr, Text::CStringNN serverName, NN<const Text::MIMEObj::MailMessage> mail) = 0;
			virtual Optional<IO::StreamData> OpenEmailData(Int64 id) = 0;
			virtual UnsafeArrayOpt<const UTF8Char> GetEmailUid(Int64 id) = 0;
			virtual UIntOS GetRcptList(Int64 id, NN<Data::ArrayListStringNN> rcptList) = 0;
			virtual Net::Email::MailController::RemoveStatus RemoveMessage(Text::CString userName, UIntOS msgIndex) = 0;
			virtual Optional<Net::Email::EmailStore::EmailInfo> GetEmailByIndex(Text::CString userName, UIntOS msgIndex) = 0;
			virtual void GetMessageStat(Text::CString userName, NN<MessageStat> stat) = 0;
			virtual UIntOS GetUnreadIndices(Text::CString userName, NN<Data::ArrayListNative<UIntOS>> indices) = 0;
		};
	}
}
#endif
