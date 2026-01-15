#include "Stdafx.h"
#include "IO/StmData/MemoryDataRef.h"
#include "UI/ClipboardUtil.h"

Optional<Media::ImageList> UI::ClipboardUtil::LoadImage(NN<UI::Clipboard> clipboard, NN<Parser::ParserList> parsers)
{
	Data::ArrayListNative<UInt32> dataTypes;
	clipboard->GetDataFormats(dataTypes);
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i = 0;
	UOSInt j = dataTypes.GetCount();
	while (i < j)
	{
		sptr = UI::Clipboard::GetFormatName(dataTypes.GetItem(i), sbuff, sizeof(sbuff));
		if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("image/png")) || Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("image/jpeg")) || Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("image/bmp")) || Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("image/tiff")))
		{
			NN<Media::ImageList> imgList;
			NN<Data::ByteBuffer> buff;
			if (clipboard->GetDataRAW(dataTypes.GetItem(i)).SetTo(buff))
			{
				IO::StmData::MemoryDataRef memData(buff->Arr(), buff->GetSize());
				if (Optional<Media::ImageList>::ConvertFrom(parsers->ParseFileType(memData, IO::ParserType::ImageList)).SetTo(imgList))
				{
					buff.Delete();
					return imgList;
				}
				buff.Delete();
			}
		}
		i++;
	}
	return nullptr;
}
