#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CodeImageGen/CodeImageGen.h"
#include "Media/CodeImageGen/EAN13CodeImageGen.h"
#include "Media/CodeImageGen/EAN2CodeImageGen.h"
#include "Media/CodeImageGen/EAN5CodeImageGen.h"
#include "Media/CodeImageGen/EAN8CodeImageGen.h"
#include "Media/CodeImageGen/QRCodeImageGen.h"

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
	case Media::CodeImageGen::CodeImageGen::CT_QRCODE:
		NEW_CLASS(codeImgGen, Media::CodeImageGen::QRCodeImageGen());
		break;
	}
	return codeImgGen;
}

Text::CStringNN Media::CodeImageGen::CodeImageGen::GetCodeName(CodeType codeType)
{
	switch (codeType)
	{
	case Media::CodeImageGen::CodeImageGen::CT_EAN13:
		return CSTR("EAN-13");
	case Media::CodeImageGen::CodeImageGen::CT_EAN8:
		return CSTR("EAN-8");
	case Media::CodeImageGen::CodeImageGen::CT_EAN5:
		return CSTR("EAN-5");
	case Media::CodeImageGen::CodeImageGen::CT_EAN2:
		return CSTR("EAN-2");
	case Media::CodeImageGen::CodeImageGen::CT_QRCODE:
		return CSTR("QR Code");
	default:
		return CSTR("Unknown");
	}
}
