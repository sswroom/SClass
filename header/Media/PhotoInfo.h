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
		Text::String *make;
		Text::String *model;
		UInt32 width;
		UInt32 height;
		Double fNumber;
		Double expTime;
		UInt32 isoRating;
		Text::String *lens;
		Double focalLength;
		Data::DateTime *photoDate;

		void ParseXMF(Text::XMLDocument *xmf);
		Double ParseFraction(Text::String *s);
	public:
		PhotoInfo(IO::StreamData *fd);
		~PhotoInfo();

		Bool HasInfo() const;
		Bool GetPhotoDate(Data::DateTime *dt) const;

		void ToString(Text::StringBuilderUTF8 *sb) const;
	};
};
#endif
