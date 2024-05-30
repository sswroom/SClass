#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/UTF8Util.h"

Bool Text::UTF8Util::ValidStr(UnsafeArray<const UTF8Char> s)
{
	Bool valid = true;
	UTF8Char c;
	while (true)
	{
		c = *s++;
		if (c == 0)
			break;
		else if (c < 0x80)
		{

		}
		else if (c < 0xc0)
		{
			valid = false;
			break;
		}
		else if (c < 0xe0)
		{
			c = *s++;
			if ((c & 0xc0) != 0x80)
			{
				valid = false;
				break;
			}
		}
		else if (c < 0xf0)
		{
			c = *s++;
			if ((c & 0xc0) != 0x80)
			{
				valid = false;
				break;
			}
			c = *s++;
			if ((c & 0xc0) != 0x80)
			{
				valid = false;
				break;
			}
		}
		else if (c < 0xf8)
		{
			c = *s++;
			if ((c & 0xc0) != 0x80)
			{
				valid = false;
				break;
			}
			c = *s++;
			if ((c & 0xc0) != 0x80)
			{
				valid = false;
				break;
			}
			c = *s++;
			if ((c & 0xc0) != 0x80)
			{
				valid = false;
				break;
			}
		}
		else if (c < 0xfc)
		{
			c = *s++;
			if ((c & 0xc0) != 0x80)
			{
				valid = false;
				break;
			}
			c = *s++;
			if ((c & 0xc0) != 0x80)
			{
				valid = false;
				break;
			}
			c = *s++;
			if ((c & 0xc0) != 0x80)
			{
				valid = false;
				break;
			}
			c = *s++;
			if ((c & 0xc0) != 0x80)
			{
				valid = false;
				break;
			}
		}
		else if (c < 0xfe)
		{
			c = *s++;
			if ((c & 0xc0) != 0x80)
			{
				valid = false;
				break;
			}
			c = *s++;
			if ((c & 0xc0) != 0x80)
			{
				valid = false;
				break;
			}
			c = *s++;
			if ((c & 0xc0) != 0x80)
			{
				valid = false;
				break;
			}
			c = *s++;
			if ((c & 0xc0) != 0x80)
			{
				valid = false;
				break;
			}
			c = *s++;
			if ((c & 0xc0) != 0x80)
			{
				valid = false;
				break;
			}
		}
		else
		{
			valid = false;
			break;
		}
	}
	return valid;
}
