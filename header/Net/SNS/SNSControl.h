#ifndef _SM_NET_SNS_SNSCONTROL
#define _SM_NET_SNS_SNSCONTROL
#include "Data/ArrayList.h"
#include "Text/CString.h"
#include "Text/String.h"

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
				NotNullPtr<Text::String> id;
				Int64 msgTime;
				Text::String *title;
				NotNullPtr<Text::String> message;
				Text::String *msgLink;
				Text::String *imgURL;
				Text::String *videoURL;
			} SNSItem;
			
		public:
			virtual ~SNSControl() {};

			virtual Bool IsError() = 0;
			virtual SNSType GetSNSType() = 0;
			virtual NotNullPtr<Text::String> GetChannelId() const = 0;
			virtual NotNullPtr<Text::String> GetName() const = 0;
			virtual UTF8Char *GetDirName(UTF8Char *dirName) = 0;
			virtual UOSInt GetCurrItems(Data::ArrayList<SNSItem*> *itemList) = 0;
			virtual UTF8Char *GetItemShortId(UTF8Char *buff, SNSItem *item) = 0;
			virtual Int32 GetMinIntevalMS() = 0;
			virtual Bool Reload() = 0;

			static Text::CString SNSTypeGetName(SNSType snsType);
			static SNSType SNSTypeFromName(Text::CString name);

			static SNSItem *CreateItem(NotNullPtr<Text::String> id, Int64 msgTime, Text::String *title, NotNullPtr<Text::String> message, Text::String *msgLink, Text::String *imgURL, Text::String *videoURL);
			static void FreeItem(SNSItem *item);
		};
	}
}
#endif
