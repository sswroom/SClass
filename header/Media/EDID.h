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
			UInt8 dispPhysicalW;
			UInt8 dispPhysicalH;

			UTF8Char monitorName[14];
			UTF8Char monitorSN[14];
			Double gamma;
			Double rx;
			Double ry;
			Double gx;
			Double gy;
			Double bx;
			Double by;
			Double wx;
			Double wy;
		} EDIDInfo;
		static Bool Parse(const UInt8 *edidBuff, EDIDInfo *info);
		static Bool SetColorProfile(EDIDInfo *info, Media::ColorProfile *cp);
	};
}
#endif
