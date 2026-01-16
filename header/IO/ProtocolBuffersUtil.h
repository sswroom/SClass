#ifndef _SM_IO_PROTOCOLBUFFERSUTIL
#define _SM_IO_PROTOCOLBUFFERSUTIL
#include "IO/MemoryStream.h"
#include "Map/OSM/OSMData.h"
#include "Text/CString.h" 

namespace IO
{
	class ProtocolBuffersUtil
	{
	public:
		enum class FieldType
		{
			Int32,
			Int64,
			SInt32,
			SInt64,
			UInt32,
			UInt64,
			String,
			Bytes,
			Fixed32,
			Fixed64,
			SFixed32,
			SFixed64,
			Float,
			Double,
			Bool,
			Enum,
			SubMsg
		};

		static UIntOS ReadVarUInt(UnsafeArray<const UInt8> buff, UIntOS buffOfst, OutParam<UInt64> val);
		static UIntOS ReadVarInt(UnsafeArray<const UInt8> buff, UIntOS buffOfst, OutParam<Int64> val);
		static UIntOS ReadVarSInt(UnsafeArray<const UInt8> buff, UIntOS buffOfst, OutParam<Int64> val);
		static Int64 ToSInt64(UInt64 val);
		static Text::CStringNN WireTypeGetName(UInt8 wireType);
		static Optional<IO::MemoryStream> DecompressBlob(Data::ByteArrayR blobData);
		static Bool ParseOSMHeader(NN<IO::MemoryStream> blobStm, NN<Map::OSM::OSMData> osmData);
		static Bool ParseOSMData(NN<IO::MemoryStream> blobStm, NN<Map::OSM::OSMData> osmData);
	};
}
#endif
