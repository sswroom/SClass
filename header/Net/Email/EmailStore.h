#ifndef _SM_NET_EMAIL_EMAILSTORE
#define _SM_NET_EMAIL_EMAILSTORE
#include "Data/StringUTF8Map.h"
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
				NotNullPtr<Text::String> fromAddr;
				UOSInt fileSize;
				Bool isDeleted;
			};

			struct MessageStat
			{
				UOSInt unreadCount;
				UInt64 unreadSize;
			};
		protected:
			Data::ArrayList<EmailInfo*> mailList;
			Sync::Mutex mailMut;

		public:
			EmailStore();
			virtual ~EmailStore();

			UOSInt GetAllEmails(Data::ArrayList<EmailInfo*> *emailList);

			virtual Int64 NextEmailId() = 0;
			virtual Bool NewEmail(Int64 id, NotNullPtr<const Net::SocketUtil::AddressInfo> remoteAddr, Text::CStringNN serverName, NotNullPtr<const Net::Email::SMTPServer::MailStatus> mail) = 0;
			virtual Bool NewEmail(Int64 id, NotNullPtr<const Net::SocketUtil::AddressInfo> remoteAddr, Text::CStringNN serverName, NotNullPtr<const Text::MIMEObj::MailMessage> mail) = 0;
			virtual IO::StreamData *OpenEmailData(Int64 id) = 0;
			virtual const UTF8Char *GetEmailUid(Int64 id) = 0;
			virtual UOSInt GetRcptList(Int64 id, NotNullPtr<Data::ArrayListStringNN> rcptList) = 0;
			virtual Net::Email::MailController::RemoveStatus RemoveMessage(Text::CString userName, UOSInt msgIndex) = 0;
			virtual Net::Email::EmailStore::EmailInfo *GetEmailByIndex(Text::CString userName, UOSInt msgIndex) = 0;
			virtual void GetMessageStat(Text::CString userName, MessageStat *stat) = 0;
			virtual UOSInt GetUnreadIndices(Text::CString userName, Data::ArrayList<UOSInt> *indices) = 0;
		};
	}
}
#endif
