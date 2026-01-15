#ifndef _SM_DATA_COMPRESS_PACKBITS
#define _SM_DATA_COMPRESS_PACKBITS
namespace Data
{
	namespace Compress
	{
		class PackBits
		{
		public:
			static Bool Decompress(UnsafeArray<UInt8> destBuff, OutParam<UIntOS> destBuffSize, UnsafeArray<UInt8> srcBuff, UIntOS srcBuffSize);
		};
	}
}

#endif
