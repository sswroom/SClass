#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/NullIOPin.h"
#include "Text/MyString.h"

IO::NullIOPin::NullIOPin()
{
}

IO::NullIOPin::~NullIOPin()
{
}

Bool IO::NullIOPin::IsPinHigh()
{
	return false;
}

Bool IO::NullIOPin::IsPinOutput()
{
	return false;
}

void IO::NullIOPin::SetPinOutput(Bool isOutput)
{
}

void IO::NullIOPin::SetPinState(Bool isHigh)
{
}

Bool IO::NullIOPin::SetPullType(PullType pt)
{
	return false;
}

UTF8Char *IO::NullIOPin::GetName(UTF8Char *buff)
{
	return Text::StrConcatC(buff, UTF8STRC("NullIOPin"));
}
