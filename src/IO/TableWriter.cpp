#include "Stdafx.h"
#include "IO/TableWriter.h"

IO::TableWriter::TableWriter(NN<IO::SeekableStream> stm)
{
	this->stm = stm;
}

IO::TableWriter::~TableWriter()
{
}
