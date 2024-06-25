#ifndef _SM_MEDIA_EDID
#define _SM_MEDIA_EDID
#include "Media/ColorProfile.h"

namespace Media
{
	class EDID
	{
	public:
		typedef struct
		{
			UTF8Char vendorName[4];
			UInt16 productCode;
			UInt32 sn;
			UInt8 weekOfManu;
			Int32 yearOfManu;
			UInt8 edidVer;
			UInt8 edidRev;
			UInt32 pixelW;
			UInt32 pixelH;
			UInt32 bitPerColor;
			UInt32 dispPhysicalW_mm;
			UInt32 dispPhysicalH_mm;

			UTF8Char monitorName[14];
			UTF8Char monitorSN[14];
			UTF8Char monitorOther[14];
			Double gamma;
			Math::Double2D r;
			Math::Double2D g;
			Math::Double2D b;
			Math::Double2D w;
		} EDIDInfo;
		static Bool Parse(UnsafeArray<const UInt8> edidBuff, NN<EDIDInfo> info);
		static void ParseDescriptor(NN<EDIDInfo> info, const UInt8 *descriptor);
		static Bool SetColorProfile(NN<EDIDInfo> info, NN<Media::ColorProfile> cp);
	};
}
#endif
