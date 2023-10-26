#include "Stdafx.h"
#include "Math/WKBWriter.h"

#include <stdio.h>

Math::WKBWriter::WKBWriter()
{
}

Math::WKBWriter::~WKBWriter()
{
}

Bool Math::WKBWriter::Write(NotNullPtr<IO::Stream> stm, NotNullPtr<Math::Geometry::Vector2D> vec)
{
	printf("WKBWriter: Unsupported type: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()).v);
	return false;
}
