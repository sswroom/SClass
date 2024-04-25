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
			NN<Text::String> key;
			NN<Text::String> addr;
			Double lat;
			Double lon;
			NN<Text::String> url;
		} ImageInfo;

		typedef struct
		{
			NN<Text::String> groupName;
			Data::ArrayListNN<ImageInfo> *imageList;
		} GroupInfo;

	private:
		Data::FastStringMapNN<GroupInfo> groupMap;

		void Init(NN<Text::EncodingFactory> encFact, const UInt8 *buff, UOSInt buffSize);
	public:
		HKTrafficImage(NN<Text::EncodingFactory> encFact, const UInt8 *buff, UOSInt buffSize);
		HKTrafficImage(NN<Text::EncodingFactory> encFact, Text::CStringNN fileName);
		~HKTrafficImage();

		UOSInt GetGroups(NN<Data::ArrayListNN<GroupInfo>> groups);
	};
}
#endif
