#include "Stdafx.h"
#include "Media/FontRenderer.h"

Media::FontRenderer::FontRenderer(const UTF8Char *sourceName) : IO::ParsedObject(sourceName)
{
}

Media::FontRenderer::~FontRenderer()
{
}

IO::ParsedObject::ParserType Media::FontRenderer::GetParserType()
{
	return IO::ParsedObject::PT_FONT_RENDERER;
}
