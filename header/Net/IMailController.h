#ifndef _SM_NET_IMAILCONTROLLER
#define _SM_NET_IMAILCONTROLLER
#include "Data/ArrayList.h"
#include "IO/Stream.h"

namespace Net
{
	class IMailController
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
			const UTF8Char *uid;
		} MessageInfo;
	public:
		virtual ~IMailController() {};

		virtual Bool Login(const Char *user, const Char *pwd, Int32 *userId) = 0;
		virtual UOSInt GetMessageStat(Int32 userId, UOSInt *size) = 0;
		virtual Bool GetUnreadList(Int32 userId, Data::ArrayList<UInt32> *unreadList) = 0;
		virtual Bool GetMessageInfo(Int32 userId, UInt32 msgId, MessageInfo *info) = 0;
		virtual Bool GetMessageContent(Int32 userId, UInt32 msgId, IO::Stream *stm) = 0;
		virtual RemoveStatus RemoveMessage(Int32 userId, UInt32 msgId) = 0;
	};
}
#endif