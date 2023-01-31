#include "Stdafx.h"
#include "Text/Builder/HTMLBodyBuilder.h"

Text::Builder::HTMLBodyBuilder::HTMLBodyBuilder(Text::StringBuilderUTF8 *sb)
{
	this->sb = sb;
}

Text::Builder::HTMLBodyBuilder::~HTMLBodyBuilder()
{
}
