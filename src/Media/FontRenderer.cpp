#include "Stdafx.h"
#include "Media/FontRenderer.h"

Media::FontRenderer::FontRenderer(Text::String *sourceName) : IO::ParsedObject(sourceName)
{
}

Media::FontRenderer::~FontRenderer()
{
}

IO::ParserType Media::FontRenderer::GetParserType()
{
	return IO::ParserType::FontRenderer;
}
