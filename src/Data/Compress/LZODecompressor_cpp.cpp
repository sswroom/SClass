#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Data/Compress/LZODecompressor.h"

Bool LZODecompressor_Decompress(const UInt8 *in, UOSInt in_len, UInt8 *out, UOSInt *out_len)
{
	const UInt8 * const ip_end = in + in_len;
	const UInt8 *ip = in;
	UInt8 *op = out;
	const UInt8 *m_pos;
	OSInt t;

	*out_len = 0;

	if (*ip > 17)
	{
		t = *ip++ - 17;
		if (t < 4)
			goto match_next;
		if ((ip_end - ip) < (t + 1))
			goto input_overrun;
		while (t-- > 0)
		{
			*op++ = *ip++;
		}
		goto first_literal_run;
	}

	while (ip < ip_end)
	{
		t = *ip++;
		if (t >= 16)
			goto match;
		if (t == 0)
		{
			if (ip >= ip_end)
				goto input_overrun;
			while (*ip == 0)
			{
				t += 255;
				ip++;
				if (ip >= ip_end)
					goto input_overrun;
			}
			t += 15 + *ip++;
		}
		if ((ip_end - ip) < (t + 4))
			goto input_overrun;

		t += 3;
		while (t >= 4)
		{
			WriteInt32(op, ReadInt32(ip));
			op += 4;
			ip += 4;
			t -= 4;
		}
		while (t > 0)
		{
			*op++ = *ip++;
			t--;
		}

first_literal_run:
		t = *ip++;
		if (t >= 16)
			goto match;
		m_pos = op - (1 + 0x0800);
		m_pos -= t >> 2;
		m_pos -= *ip++ << 2;
		
		if (m_pos < out || m_pos >= op)
			goto lookbehind_overrun;

        WriteInt16(op, ReadInt16(m_pos));
		op[2] = m_pos[2];
		op += 3;
		m_pos += 2;

		goto match_done;

		while (ip < ip_end)
		{
match:
			if (t >= 64)
			{
				m_pos = op - 1;
				m_pos -= (t >> 2) & 7;
				m_pos -= *ip++ << 3;
				t = (t >> 5) - 1;
				if (m_pos < out || m_pos >= op)
					goto lookbehind_overrun;
				goto copy_match;
            }
			else if (t >= 32)
			{
				t &= 31;
				if (t == 0)
				{
					if (ip >= ip_end)
						goto input_overrun;
					while (*ip == 0)
					{
						t += 255;
						ip++;
						if (ip >= ip_end)
							goto input_overrun;
					}
					t += 31 + *ip++;
				}
				m_pos = op - 1;
				m_pos -= ReadUInt16(ip) >> 2;
				ip += 2;
			}
			else if (t >= 16)
			{
				m_pos = op;
				m_pos -= (t & 8) << 11;

				t &= 7;
				if (t == 0)
				{
					if (ip >= ip_end)
						goto input_overrun;
					while (*ip == 0)
					{
						t += 255;
						ip++;
						if (ip >= ip_end)
							goto input_overrun;
					}
					t += 7 + *ip++;
				}
				m_pos -= ReadUInt16(ip) >> 2;
				ip += 2;
				if (m_pos == op)
					goto eof_found;
				m_pos -= 0x4000;
			}
			else
			{
				m_pos = op - 1;
				m_pos -= t >> 2;
				m_pos -= *ip++ << 2;
				
				if (m_pos < out || m_pos >= op)
					goto lookbehind_overrun;

				WriteInt16(op, ReadInt16(m_pos));
				op += 2;
				m_pos++;
				goto match_done;
			}

			if (m_pos < out || m_pos >= op)
				goto lookbehind_overrun;

			if (t >= 2 * 4 - (3 - 1) && (op - m_pos) >= 4)
			{
				WriteInt32(op, ReadInt32(m_pos));
				op += 4;
				m_pos += 4;
				t -= 4 - (3 - 1);
				while (t >= 4)
				{
					WriteInt32(op, ReadInt32(m_pos));
					op += 4;
					m_pos += 4;
					t -= 4;
				}
				while (t-- > 0)
				{
					*op++ = *m_pos++;
				}
			}
			else
			{
copy_match:
				*op++ = *m_pos++;
				*op++ = *m_pos++;
				while (t-- > 0)
				{
					*op++ = *m_pos++;
				}
			}
match_done:
			t = ip[-2] & 3;
			if (t == 0)
				break;
match_next:
			if ((ip_end - ip) < (t + 1))
				goto input_overrun;
			
			*op++ = *ip++;
			if (t > 1)
			{
				*op++ = *ip++;
				if (t > 2)
					*op++ = *ip++;
			}
			
			t = *ip++;
		}
	}

	*out_len = (UOSInt)(op - out);
	return false; //LZO_E_EOF_NOT_FOUND;

eof_found:
	*out_len = (UOSInt)(op - out);
	return ip == ip_end;

input_overrun:
	*out_len = (UOSInt)(op - out);
	return false; //LZO_E_INPUT_OVERRUN;

lookbehind_overrun:
	*out_len = (UOSInt)(op - out);
	return false; //LZO_E_LOOKBEHIND_OVERRUN;
}

UOSInt LZODecompressor_CalcDecSize(const UInt8 *in, UOSInt in_len)
{
	const UInt8 * const ip_end = in + in_len;
	const UInt8 *ip = in;
	const UInt8 *op = ip_end;
	const UInt8 *m_pos;
	OSInt t;

	if (*ip > 17)
	{
		t = *ip++ - 17;
		if (t < 4)
			goto match_next;
		if ((ip_end - ip) < (t + 1))
			goto input_overrun;
		op += t;
		ip += t;
		goto first_literal_run;
	}

	while (ip < ip_end)
	{
		t = *ip++;
		if (t >= 16)
			goto match;
		if (t == 0)
		{
			if (ip >= ip_end)
				goto input_overrun;
			while (*ip == 0)
			{
				t += 255;
				ip++;
				if (ip >= ip_end)
					goto input_overrun;
			}
			t += 15 + *ip++;
		}
		if ((ip_end - ip) < (t + 4))
			goto input_overrun;

		t += 3;
		op += t;
		ip += t;

first_literal_run:
		t = *ip++;
		if (t >= 16)
			goto match;
		m_pos = op - (1 + 0x0800);
		m_pos -= t >> 2;
		m_pos -= *ip++ << 2;
		
		if (m_pos < ip_end || m_pos >= op)
			goto lookbehind_overrun;

		op += 3;
		m_pos += 2;

		goto match_done;

		while (ip < ip_end)
		{
match:
			if (t >= 64)
			{
				m_pos = op - 1;
				m_pos -= (t >> 2) & 7;
				m_pos -= *ip++ << 3;
				t = (t >> 5) - 1;
				if (m_pos < ip_end || m_pos >= op)
					goto lookbehind_overrun;
				goto copy_match;
            }
			else if (t >= 32)
			{
				t &= 31;
				if (t == 0)
				{
					if (ip >= ip_end)
						goto input_overrun;
					while (*ip == 0)
					{
						t += 255;
						ip++;
						if (ip >= ip_end)
							goto input_overrun;
					}
					t += 31 + *ip++;
				}
				m_pos = op - 1;
				m_pos -= ReadUInt16(ip) >> 2;
				ip += 2;
			}
			else if (t >= 16)
			{
				m_pos = op;
				m_pos -= (t & 8) << 11;

				t &= 7;
				if (t == 0)
				{
					if (ip >= ip_end)
						goto input_overrun;
					while (*ip == 0)
					{
						t += 255;
						ip++;
						if (ip >= ip_end)
							goto input_overrun;
					}
					t += 7 + *ip++;
				}
				m_pos -= ReadUInt16(ip) >> 2;
				ip += 2;
				if (m_pos == op)
					goto eof_found;
				m_pos -= 0x4000;
			}
			else
			{
				m_pos = op - 1;
				m_pos -= t >> 2;
				m_pos -= *ip++ << 2;
				
				if (m_pos < ip_end || m_pos >= op)
					goto lookbehind_overrun;

				op += 2;
				m_pos++;
				goto match_done;
			}

			if (m_pos < ip_end || m_pos >= op)
				goto lookbehind_overrun;

			if (t >= 2 * 4 - (3 - 1) && (op - m_pos) >= 4)
			{
				t += 2;
				op += t;
				m_pos += t;
			}
			else
			{
copy_match:
				op += 2 + t;
				m_pos += 2 + t;
			}
match_done:
			t = ip[-2] & 3;
			if (t == 0)
				break;
match_next:
			if ((ip_end - ip) < (t + 1))
				goto input_overrun;
			
			op += t;
			ip += t;
			
			t = *ip++;
		}
	}

	return 0; //LZO_E_EOF_NOT_FOUND;

eof_found:
	if (ip == ip_end)
		return (UOSInt)(op - ip_end);
	return 0;

input_overrun:
	return 0; //LZO_E_INPUT_OVERRUN;

lookbehind_overrun:
	return 0; //LZO_E_LOOKBEHIND_OVERRUN;
}
