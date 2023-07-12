#ifndef _SM_NET_EMAIL_FILEEMAILSTORE
#define _SM_NET_EMAIL_FILEEMAILSTORE
#include "Data/FastMap.h"
#include "Net/Email/EmailStore.h"

namespace Net
{
	namespace Email
	{
		class FileEmailStore : public EmailStore
		{
		public:
			struct FileInfo
			{
				Int64 id;
				Data::ArrayListNN<Text::String> rcptList;
				NotNullPtr<Text::String> fileName;
				const UTF8Char *uid;
			};
		private:
			Data::FastMap<Int64, FileInfo*> fileMap;
			Sync::Mutex fileMut;
			Int64 currId;
			Sync::Mutex currIdMut;
			UOSInt recvIndex;

			FileInfo *GetFileInfo(Int64 id);
			void AddMail(const Text::MIMEObj::MailMessage *mail, UTF8Char *filePath, UTF8Char *fileNameStart, UTF8Char *filePathEnd, UInt64 fileSize);
		public:
			FileEmailStore();
			virtual ~FileEmailStore();

			virtual Int64 NextEmailId();
			virtual Bool NewEmail(Int64 id, const Net::SocketUtil::AddressInfo *remoteAddr, Text::CString serverName, const Net::Email::SMTPServer::MailStatus *mail);
			virtual Bool NewEmail(Int64 id, const Net::SocketUtil::AddressInfo *remoteAddr, Text::CString serverName, const Text::MIMEObj::MailMessage *mail);
			virtual IO::StreamData *OpenEmailData(Int64 id);
			virtual const UTF8Char *GetEmailUid(Int64 id);
			virtual UOSInt GetRcptList(Int64 id, Data::ArrayList<Text::String*> *rcptList);
			virtual Net::Email::MailController::RemoveStatus RemoveMessage(Text::CString userName, UOSInt msgIndex);
			virtual Net::Email::EmailStore::EmailInfo *GetEmailByIndex(Text::CString userName, UOSInt msgIndex);
			virtual void GetMessageStat(Text::CString userName, MessageStat *stat);
			virtual UOSInt GetUnreadIndices(Text::CString userName, Data::ArrayList<UOSInt> *indices);
		};
	}
}
#endif
