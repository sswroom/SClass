#ifndef _SM_MEDIA_PHOTOINFO
#define _SM_MEDIA_PHOTOINFO
#include "IO/IStreamData.h"
#include "Text/StringBuilderUTF.h"
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
		PhotoInfo(IO::IStreamData *fd);
		~PhotoInfo();

		Bool HasInfo();
		Bool GetPhotoDate(Data::DateTime *dt);

		void ToString(Text::StringBuilderUTF *sb);
	};
};
#endif
