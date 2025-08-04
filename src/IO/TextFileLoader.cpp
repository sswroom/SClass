#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "IO/TextFileLoader.h"

Bool IO::TextFileLoader::LoadDoubleList(Text::CStringNN fileName, NN<Data::ArrayListDbl> dblList)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Double dVal;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
		return false;
	IO::StreamReader reader(fs, 0);
	while (reader.ReadLine(sbuff, 511).SetTo(sptr))
	{
		if (!Text::StrToDouble(sbuff, dVal))
		{
			return false;
		}
		dblList->Add(dVal);
	}
	return true;
}
