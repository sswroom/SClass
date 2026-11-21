#ifndef _SM_MEDIA_PDFUTIL
#define _SM_MEDIA_PDFUTIL
#include "Text/CString.h"

namespace Media
{
	class PDFUtil
	{
	public:
		static void SkipPDFWhitespace(UnsafeArray<const UTF8Char> str, UIntOS length, UIntOS &pos);
		static Bool ReadPDFNumberToken(UnsafeArray<const UTF8Char> str, UIntOS length, UIntOS &pos, UIntOS &start, UIntOS &end);
		static Bool IsPDFDecodeEndpoint(UnsafeArray<const UTF8Char> str, UIntOS start, UIntOS end, UInt8 endpoint);
		static Bool IsReverseDecodeArray(Text::CStringNN value);
		static void SkipIndexedWhitespace(UnsafeArray<const UTF8Char> str, UIntOS length, UIntOS &pos);
		static Bool ReadIndexedToken(UnsafeArray<const UTF8Char> str, UIntOS length, UIntOS &pos, UIntOS &start, UIntOS &end);
		static Bool ParseIndexedUInt(UnsafeArray<const UTF8Char> str, UIntOS start, UIntOS end, UIntOS &value);
		static Bool ReadIndexedReference(UnsafeArray<const UTF8Char> str, UIntOS length, UIntOS &pos, UInt32 &id);
		static Bool ValidateIndexedICC(Data::ByteArrayR bytes);
		static Bool ResolveIndexedBase(NN<Media::PDFDocument> doc, Text::CStringNN base, UIntOS &components, NN<Media::ColorProfile> color);
	};
}
#endif
	