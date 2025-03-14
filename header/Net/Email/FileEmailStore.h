#ifndef _SM_NET_EMAIL_FILEEMAILSTORE
#define _SM_NET_EMAIL_FILEEMAILSTORE
#include "Data/FastMapNN.h"
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
				Data::ArrayListStringNN rcptList;
				NN<Text::String> fileName;
				const UTF8Char *uid;
			};
		private:
			Data::FastMapNN<Int64, FileInfo> fileMap;
			Sync::Mutex fileMut;
			Int64 currId;
			Sync::Mutex currIdMut;
			UOSInt recvIndex;

			Optional<FileInfo> GetFileInfo(Int64 id);
			void AddMail(NN<const Text::MIMEObj::MailMessage> mail, UnsafeArray<UTF8Char> filePath, UnsafeArray<UTF8Char> fileNameStart, UnsafeArray<UTF8Char> filePathEnd, UInt64 fileSize);
		public:
			FileEmailStore();
			virtual ~FileEmailStore();

			virtual Int64 NextEmailId();
			virtual Bool NewEmail(Int64 id, NN<const Net::SocketUtil::AddressInfo> remoteAddr, Text::CStringNN serverName, NN<const Net::Email::SMTPServer::MailStatus> mail);
			virtual Bool NewEmail(Int64 id, NN<const Net::SocketUtil::AddressInfo> remoteAddr, Text::CStringNN serverName, NN<const Text::MIMEObj::MailMessage> mail);
			virtual Optional<IO::StreamData> OpenEmailData(Int64 id);
			virtual UnsafeArrayOpt<const UTF8Char> GetEmailUid(Int64 id);
			virtual UOSInt GetRcptList(Int64 id, NN<Data::ArrayListStringNN> rcptList);
			virtual Net::Email::MailController::RemoveStatus RemoveMessage(Text::CString userName, UOSInt msgIndex);
			virtual Optional<Net::Email::EmailStore::EmailInfo> GetEmailByIndex(Text::CString userName, UOSInt msgIndex);
			virtual void GetMessageStat(Text::CString userName, NN<MessageStat> stat);
			virtual UOSInt GetUnreadIndices(Text::CString userName, NN<Data::ArrayList<UOSInt>> indices);
		};
	}
}
#endif
