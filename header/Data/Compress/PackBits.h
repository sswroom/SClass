#ifndef _SM_DATA_COMPRESS_PACKBITS
#define _SM_DATA_COMPRESS_PACKBITS
namespace Data
{
	namespace Compress
	{
		class PackBits
		{
		public:
			static Bool Decompress(UInt8 *destBuff, OSInt *destBuffSize, UInt8 *srcBuff, OSInt srcBuffSize);
		};
	};
};

#endif
