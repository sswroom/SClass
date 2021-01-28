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
		NEW_CLASS(decomp, Data::Compress::Inflate());
		break;
	case CM_LZO:
		NEW_CLASS(decomp, Data::Compress::LZODecompressor());
		break;
	default:
		decomp = 0;
		break;
	}
	return decomp;
}

const UTF8Char *Data::Compress::Decompressor::GetCompMethName(Data::Compress::Decompressor::CompressMethod compMethod)
{
	switch (compMethod)
	{
	case CM_DEFLATE:
		return (const UTF8Char*)"Deflate";
	case CM_UNCOMPRESSED:
		return (const UTF8Char*)"Uncompressed";
	case CM_MLH:
		return (const UTF8Char*)"MLH";
	case CM_LZO:
		return (const UTF8Char*)"LZO";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
