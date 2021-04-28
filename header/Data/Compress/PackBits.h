#ifndef _SM_DATA_COMPRESS_PACKBITS
#define _SM_DATA_COMPRESS_PACKBITS
namespace Data
{
	namespace Compress
	{
		class PackBits
		{
		public:
			static Bool Decompress(UInt8 *destBuff, UOSInt *destBuffSize, UInt8 *srcBuff, UOSInt srcBuffSize);
		};
	}
}

#endif
