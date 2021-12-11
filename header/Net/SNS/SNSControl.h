#ifndef _SM_NET_SNS_SNSCONTROL
#define _SM_NET_SNS_SNSCONTROL
#include "Data/ArrayList.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF.h"

namespace Net
{
	namespace SNS
	{
		class SNSControl
		{
		public:
			typedef enum
			{
				ST_UNKNOWN,
				ST_RSS,
				ST_TWITTER,
				ST_7GOGO,
				ST_INSTAGRAM,

				ST_FIRST = ST_RSS,
				ST_LAST = ST_INSTAGRAM
			} SNSType;

			typedef struct
			{
				Text::String *id;
				Int64 msgTime;
				Text::String *title;
				Text::String *message;
				Text::String *msgLink;
				Text::String *imgURL;
				Text::String *videoURL;
			} SNSItem;
			
		public:
			virtual ~SNSControl() {};

			virtual Bool IsError() = 0;
			virtual SNSType GetSNSType() = 0;
			virtual Text::String *GetChannelId() = 0;
			virtual Text::String *GetName() = 0;
			virtual UTF8Char *GetDirName(UTF8Char *dirName) = 0;
			virtual UOSInt GetCurrItems(Data::ArrayList<SNSItem*> *itemList) = 0;
			virtual UTF8Char *GetItemShortId(UTF8Char *buff, SNSItem *item) = 0;
			virtual Int32 GetMinIntevalMS() = 0;
			virtual Bool Reload() = 0;

			static const UTF8Char *SNSTypeGetName(SNSType snsType);
			static SNSType SNSTypeFromName(const UTF8Char *name);

			static SNSItem *CreateItem(Text::String *id, Int64 msgTime, Text::String *title, Text::String *message, Text::String *msgLink, Text::String *imgURL, Text::String *videoURL);
			static void FreeItem(SNSItem *item);
		};
	}
}
#endif
