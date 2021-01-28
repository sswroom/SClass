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
		const UTF8Char *make;
		const UTF8Char *model;
		Int32 width;
		Int32 height;
		Double fNumber;
		Double expTime;
		Int32 isoRating;
		const UTF8Char *lens;
		Double focalLength;
		Data::DateTime *photoDate;

		void ParseXMF(Text::XMLDocument *xmf);
		Double ParseFraction(const UTF8Char *s);
	public:
		PhotoInfo(IO::IStreamData *fd);
		~PhotoInfo();

		Bool HasInfo();
		Bool GetPhotoDate(Data::DateTime *dt);

		void ToString(Text::StringBuilderUTF *sb);
	};
};
#endif
