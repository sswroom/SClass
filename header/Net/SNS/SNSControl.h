#ifndef _SM_NET_SNS_SNSCONTROL
#define _SM_NET_SNS_SNSCONTROL
#include "Data/ArrayListNN.h"
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
				NN<Text::String> id;
				Int64 msgTime;
				Text::String *title;
				NN<Text::String> message;
				Text::String *msgLink;
				Text::String *imgURL;
				Text::String *videoURL;
			} SNSItem;
			
		public:
			virtual ~SNSControl() {};

			virtual Bool IsError() = 0;
			virtual SNSType GetSNSType() = 0;
			virtual NN<Text::String> GetChannelId() const = 0;
			virtual NN<Text::String> GetName() const = 0;
			virtual UTF8Char *GetDirName(UTF8Char *dirName) = 0;
			virtual UOSInt GetCurrItems(NN<Data::ArrayListNN<SNSItem>> itemList) = 0;
			virtual UTF8Char *GetItemShortId(UTF8Char *buff, NN<SNSItem> item) = 0;
			virtual Int32 GetMinIntevalMS() = 0;
			virtual Bool Reload() = 0;

			static Text::CStringNN SNSTypeGetName(SNSType snsType);
			static SNSType SNSTypeFromName(Text::CStringNN name);

			static NN<SNSItem> CreateItem(NN<Text::String> id, Int64 msgTime, Text::String *title, NN<Text::String> message, Text::String *msgLink, Text::String *imgURL, Text::String *videoURL);
			static void FreeItem(NN<SNSItem> item);
		};
	}
}
#endif
