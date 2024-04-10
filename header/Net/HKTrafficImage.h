#ifndef _SM_NET_HKTRAFFICIMAGE
#define _SM_NET_HKTRAFFICIMAGE
#include "Data/ArrayListNN.h"
#include "Data/FastStringMapNN.h"
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
			Data::ArrayListNN<ImageInfo> *imageList;
		} GroupInfo;

	private:
		Data::FastStringMapNN<GroupInfo> groupMap;

		void Init(NotNullPtr<Text::EncodingFactory> encFact, const UInt8 *buff, UOSInt buffSize);
	public:
		HKTrafficImage(NotNullPtr<Text::EncodingFactory> encFact, const UInt8 *buff, UOSInt buffSize);
		HKTrafficImage(NotNullPtr<Text::EncodingFactory> encFact, Text::CStringNN fileName);
		~HKTrafficImage();

		UOSInt GetGroups(NotNullPtr<Data::ArrayListNN<GroupInfo>> groups);
	};
}
#endif
