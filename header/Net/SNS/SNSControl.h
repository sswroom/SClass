#ifndef _SM_NET_SNS_SNSCONTROL
#define _SM_NET_SNS_SNSCONTROL
#include "Data/ArrayList.h"
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
				const UTF8Char *id;
				Int64 msgTime;
				const UTF8Char *title;
				const UTF8Char *message;
				const UTF8Char *msgLink;
				const UTF8Char *imgURL;
				const UTF8Char *videoURL;
			} SNSItem;
			
		public:
			virtual ~SNSControl() {};

			virtual Bool IsError() = 0;
			virtual SNSType GetSNSType() = 0;
			virtual const UTF8Char *GetChannelId() = 0;
			virtual const UTF8Char *GetName() = 0;
			virtual UTF8Char *GetDirName(UTF8Char *dirName) = 0;
			virtual OSInt GetCurrItems(Data::ArrayList<SNSItem*> *itemList) = 0;
			virtual UTF8Char *GetItemShortId(UTF8Char *buff, SNSItem *item) = 0;
			virtual Int32 GetMinIntevalMS() = 0;
			virtual Bool Reload() = 0;

			static const UTF8Char *SNSTypeGetName(SNSType snsType);
			static SNSType SNSTypeFromName(const UTF8Char *name);

			static SNSItem *CreateItem(const UTF8Char *id, Int64 msgTime, const UTF8Char *title, const UTF8Char *message, const UTF8Char *msgLink, const UTF8Char *imgURL, const UTF8Char *videoURL);
			static void FreeItem(SNSItem *item);
		};
	}
}
#endif
