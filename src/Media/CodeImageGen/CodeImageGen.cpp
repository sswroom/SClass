#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CodeImageGen/CodeImageGen.h"
#include "Media/CodeImageGen/EAN13CodeImageGen.h"
#include "Media/CodeImageGen/EAN2CodeImageGen.h"
#include "Media/CodeImageGen/EAN5CodeImageGen.h"
#include "Media/CodeImageGen/EAN8CodeImageGen.h"

Media::CodeImageGen::CodeImageGen::~CodeImageGen()
{
}

Media::CodeImageGen::CodeImageGen *Media::CodeImageGen::CodeImageGen::CreateGenerator(CodeType codeType)
{
	Media::CodeImageGen::CodeImageGen *codeImgGen = 0;
	switch (codeType)
	{
	case Media::CodeImageGen::CodeImageGen::CT_EAN13:
		NEW_CLASS(codeImgGen, Media::CodeImageGen::EAN13CodeImageGen());
		break;
	case Media::CodeImageGen::CodeImageGen::CT_EAN8:
		NEW_CLASS(codeImgGen, Media::CodeImageGen::EAN8CodeImageGen());
		break;
	case Media::CodeImageGen::CodeImageGen::CT_EAN5:
		NEW_CLASS(codeImgGen, Media::CodeImageGen::EAN5CodeImageGen());
		break;
	case Media::CodeImageGen::CodeImageGen::CT_EAN2:
		NEW_CLASS(codeImgGen, Media::CodeImageGen::EAN2CodeImageGen());
		break;
	}
	return codeImgGen;
}

const UTF8Char *Media::CodeImageGen::CodeImageGen::GetCodeName(CodeType codeType)
{
	switch (codeType)
	{
	case Media::CodeImageGen::CodeImageGen::CT_EAN13:
		return (const UTF8Char*)"EAN-13";
	case Media::CodeImageGen::CodeImageGen::CT_EAN8:
		return (const UTF8Char*)"EAN-8";
	case Media::CodeImageGen::CodeImageGen::CT_EAN5:
		return (const UTF8Char*)"EAN-5";
	case Media::CodeImageGen::CodeImageGen::CT_EAN2:
		return (const UTF8Char*)"EAN-2";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
