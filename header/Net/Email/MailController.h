#ifndef _SM_NET_EMAIL_MAILCONTROLLER
#define _SM_NET_EMAIL_MAILCONTROLLER
#include "Data/ArrayList.h"
#include "IO/Stream.h"
#include "Text/CString.h"

namespace Net
{
	namespace Email
	{
		class MailController
		{
		public:
			typedef enum
			{
				RS_NOT_FOUND,
				RS_SUCCESS,
				RS_ALREADY_DELETED
			} RemoveStatus;

			typedef struct
			{
				UOSInt size;
				UnsafeArrayOpt<const UTF8Char> uid;
			} MessageInfo;
		public:
			virtual ~MailController() {};

			virtual Bool Login(Text::CStringNN user, Text::CStringNN pwd, OutParam<Int32> userId) = 0;
			virtual UOSInt GetMessageStat(Int32 userId, OutParam<UOSInt> size) = 0;
			virtual Bool GetUnreadList(Int32 userId, NN<Data::ArrayList<UInt32>> unreadList) = 0;
			virtual Bool GetMessageInfo(Int32 userId, UInt32 msgId, NN<MessageInfo> info) = 0;
			virtual Bool GetMessageContent(Int32 userId, UInt32 msgId, NN<IO::Stream> stm) = 0;
			virtual RemoveStatus RemoveMessage(Int32 userId, UInt32 msgId) = 0;
		};
	}
}
#endif