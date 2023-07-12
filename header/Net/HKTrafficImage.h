#ifndef _SM_NET_HKTRAFFICIMAGE
#define _SM_NET_HKTRAFFICIMAGE
#include "Data/FastStringMap.h"
#include "Text/EncodingFactory.h"

namespace Net
{
	class HKTrafficImage
	{
	public:
		typedef struct
		{
			NotNullPtr<Text::String> key;
			NotNullPtr<Text::String> addr;
			Double lat;
			Double lon;
			NotNullPtr<Text::String> url;
		} ImageInfo;

		typedef struct
		{
			NotNullPtr<Text::String> groupName;
			Data::ArrayList<ImageInfo*> *imageList;
		} GroupInfo;

	private:
		Data::FastStringMap<GroupInfo*> groupMap;

		void Init(Text::EncodingFactory *encFact, const UInt8 *buff, UOSInt buffSize);
	public:
		HKTrafficImage(Text::EncodingFactory *encFact, const UInt8 *buff, UOSInt buffSize);
		HKTrafficImage(Text::EncodingFactory *encFact, Text::CString fileName);
		~HKTrafficImage();

		UOSInt GetGroups(Data::ArrayList<GroupInfo*> *groups);
	};
}
#endif
