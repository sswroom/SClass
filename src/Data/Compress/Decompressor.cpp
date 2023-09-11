#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Compress/Decompressor.h"
#include "Data/Compress/Inflate.h"
#include "Data/Compress/LZODecompressor.h"

Data::Compress::Decompressor::~Decompressor()
{
}

Data::Compress::Decompressor *Data::Compress::Decompressor::CreateDecompressor(Data::Compress::Decompressor::CompressMethod compMeth)
{
	Data::Compress::Decompressor *decomp;
	switch (compMeth)
	{
	case CM_DEFLATE:
		NEW_CLASS(decomp, Data::Compress::Inflate(false));
		break;
	case CM_LZO:
		NEW_CLASS(decomp, Data::Compress::LZODecompressor());
		break;
	case CM_UNKNOWN:
	case CM_UNCOMPRESSED:
	case CM_MLH:
	default:
		decomp = 0;
		break;
	}
	return decomp;
}

Text::CStringNN Data::Compress::Decompressor::GetCompMethName(Data::Compress::Decompressor::CompressMethod compMethod)
{
	switch (compMethod)
	{
	case CM_DEFLATE:
		return CSTR("Deflate");
	case CM_UNCOMPRESSED:
		return CSTR("Uncompressed");
	case CM_MLH:
		return CSTR("MLH");
	case CM_LZO:
		return CSTR("LZO");
	case CM_UNKNOWN:
	default:
		return CSTR("Unknown");
	}
}
