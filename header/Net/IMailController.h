#ifndef _SM_NET_IMAILCONTROLLER
#define _SM_NET_IMAILCONTROLLER
#include "Data/ArrayList.h"
#include "IO/Stream.h"

namespace Net
{
	class IMailController
	{
	public:
		typedef struct
		{
			Int32 size;
			const UTF8Char *uid;
		} MessageInfo;
	public:
		virtual ~IMailController() {};

		virtual Bool Login(const Char *user, const Char *pwd, Int32 *userId) = 0;
		virtual OSInt GetMessageStat(Int32 userId, OSInt *size) = 0;
		virtual Bool GetUnreadList(Int32 userId, Data::ArrayList<Int32> *unreadList) = 0;
		virtual Bool GetMessageInfo(Int32 userId, Int32 msgId, MessageInfo *info) = 0;
		virtual Bool GetMessageContent(Int32 userId, Int32 msgId, IO::Stream *stm) = 0;
		virtual Int32 RemoveMessage(Int32 userId, Int32 msgId) = 0; //return: 0 = no message, 1 = success, 2 = already deleted
	};
};
#endif