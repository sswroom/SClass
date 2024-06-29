#ifndef _SM_MEDIA_PHOTOINFO
#define _SM_MEDIA_PHOTOINFO
#include "IO/StreamData.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLDOM.h"

namespace Media
{
	class PhotoInfo
	{
	private:
		Bool succ;
		Optional<Text::String> make;
		Optional<Text::String> model;
		UInt32 width;
		UInt32 height;
		Double fNumber;
		Double expTime;
		UInt32 isoRating;
		Optional<Text::String> lens;
		Double focalLength;
		Data::DateTime *photoDate;

		void ParseXMF(NN<Text::XMLDocument> xmf);
		Double ParseFraction(NN<Text::String> s);
	public:
		PhotoInfo(NN<IO::StreamData> fd);
		~PhotoInfo();

		Bool HasInfo() const;
		Bool GetPhotoDate(NN<Data::DateTime> dt) const;
		UInt32 GetWidth() const;
		UInt32 GetHeight() const;

		void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};
}
#endif
