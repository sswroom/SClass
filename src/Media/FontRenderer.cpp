#include "Stdafx.h"
#include "Media/FontRenderer.h"

Media::FontRenderer::FontRenderer(NN<Text::String> sourceName) : IO::ParsedObject(sourceName)
{
}

Media::FontRenderer::~FontRenderer()
{
}

IO::ParserType Media::FontRenderer::GetParserType() const
{
	return IO::ParserType::FontRenderer;
}
