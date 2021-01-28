#ifndef _SM_NET_HKTRAFFICIMAGE
#define _SM_NET_HKTRAFFICIMAGE
#include "Data/StringUTF8Map.h"
#include "Text/EncodingFactory.h"

namespace Net
{
	class HKTrafficImage
	{
	public:
		typedef struct
		{
			const UTF8Char *key;
			const UTF8Char *addr;
			Double lat;
			Double lon;
			const UTF8Char *url;
		} ImageInfo;

		typedef struct
		{
			const UTF8Char *groupName;
			Data::ArrayList<ImageInfo*> *imageList;
		} GroupInfo;

	private:
		Data::StringUTF8Map<GroupInfo*> *groupMap;

		void Init(Text::EncodingFactory *encFact, const UInt8 *buff, UOSInt buffSize);
	public:
		HKTrafficImage(Text::EncodingFactory *encFact, const UInt8 *buff, UOSInt buffSize);
		HKTrafficImage(Text::EncodingFactory *encFact, const UTF8Char *fileName);
		~HKTrafficImage();

		Data::ArrayList<GroupInfo*> *GetGroups();
	};
}
#endif
