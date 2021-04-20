#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/TextEnc/Punycode.h"

#define BASE 36
#define TMIN 1
#define TMAX 26
#define SKEW 38
#define DAMP 700

UOSInt Text::TextEnc::Punycode::Adapt(UOSInt delta, UOSInt numPoints, Bool firstTime)
{
	UOSInt k;

	delta = firstTime ? (delta / DAMP) : (delta >> 1);
	delta += delta / numPoints;

	k = 0;
	while (delta > ((BASE - TMIN) * TMAX) / 2)
	{
		delta /= BASE - 1;
		k += BASE;
	}

	return k + (BASE - TMIN + 1) * delta / (delta + SKEW);
}

UTF8Char *Text::TextEnc::Punycode::Encode(UTF8Char *buff, const UTF8Char *strToEnc)
{
	return Text::StrConcat(buff, strToEnc);
}

UTF8Char *Text::TextEnc::Punycode::Encode(UTF8Char *buff, const WChar *strToEnc)
{
/*	const WChar *srcPtr;
	WChar c;
	srcPtr = strToEnc;
	while (true)
	{
		c = *strToEnc++;
		if (c == 0 || c == '.')
		{

		}
	}*/

	return Text::StrWChar_UTF8(buff, strToEnc, -1);
/*
	punycode_uint n, delta, h, b, out, max_out, bias, j, m, q, k, t;

  n = initial_n;
  delta = out = 0;
  max_out = *output_length;
  bias = initial_bias;

  for (j = 0;  j < input_length;  ++j) {
    if (basic(input[j])) {
      if (max_out - out < 2) return punycode_big_output;
      output[out++] =
        case_flags ?  encode_basic(input[j], case_flags[j]) : input[j];
    }
  }

  h = b = out;

  if (b > 0) output[out++] = delimiter;

  while (h < input_length) {
    for (m = maxint, j = 0;  j < input_length;  ++j) {
      if (input[j] >= n && input[j] < m) m = input[j];
    }

    if (m - n > (maxint - delta) / (h + 1)) return punycode_overflow;
    delta += (m - n) * (h + 1);
    n = m;

    for (j = 0;  j < input_length;  ++j) {
      if (input[j] < n  ) {
        if (++delta == 0) return punycode_overflow;
      }

      if (input[j] == n) {

        for (q = delta, k = base;  ;  k += base) {
          if (out >= max_out) return punycode_big_output;
          t = k <= bias  ? tmin :     
              k >= bias + tmax ? tmax : k - bias;
          if (q < t) break;
          output[out++] = encode_digit(t + (q - t) % (base - t), 0);
          q = (q - t) / (base - t);
        }

        output[out++] = encode_digit(q, case_flags && case_flags[j]);
        bias = adapt(delta, h + 1, h == b);
        delta = 0;
        ++h;
      }
    }

    ++delta, ++n;
  }

  *output_length = out;
  return punycode_success;
  */
	return Text::StrWChar_UTF8(buff, strToEnc, -1);
}

WChar *Text::TextEnc::Punycode::Encode(WChar *buff, const WChar *strToEnc)
{
/*	const WChar *srcPtr;
	WChar c;
	srcPtr = strToEnc;
	while (true)
	{
		c = *strToEnc++;
		if (c == 0 || c == '.')
		{

		}
	}*/

	return Text::StrConcat(buff, strToEnc);
/*
	punycode_uint n, delta, h, b, out, max_out, bias, j, m, q, k, t;

  n = initial_n;
  delta = out = 0;
  max_out = *output_length;
  bias = initial_bias;

  for (j = 0;  j < input_length;  ++j) {
    if (basic(input[j])) {
      if (max_out - out < 2) return punycode_big_output;
      output[out++] =
        case_flags ?  encode_basic(input[j], case_flags[j]) : input[j];
    }
  }

  h = b = out;

  if (b > 0) output[out++] = delimiter;

  while (h < input_length) {
    for (m = maxint, j = 0;  j < input_length;  ++j) {
      if (input[j] >= n && input[j] < m) m = input[j];
    }

    if (m - n > (maxint - delta) / (h + 1)) return punycode_overflow;
    delta += (m - n) * (h + 1);
    n = m;

    for (j = 0;  j < input_length;  ++j) {
      if (input[j] < n  ) {
        if (++delta == 0) return punycode_overflow;
      }

      if (input[j] == n) {

        for (q = delta, k = base;  ;  k += base) {
          if (out >= max_out) return punycode_big_output;
          t = k <= bias  ? tmin :     
              k >= bias + tmax ? tmax : k - bias;
          if (q < t) break;
          output[out++] = encode_digit(t + (q - t) % (base - t), 0);
          q = (q - t) / (base - t);
        }

        output[out++] = encode_digit(q, case_flags && case_flags[j]);
        bias = adapt(delta, h + 1, h == b);
        delta = 0;
        ++h;
      }
    }

    ++delta, ++n;
  }

  *output_length = out;
  return punycode_success;
  */
	return Text::StrConcat(buff, strToEnc);
}

UTF8Char *Text::TextEnc::Punycode::Decode(UTF8Char *buff, const UTF8Char *strToDec)
{
	while (*strToDec)
	{
		if ((strToDec[0] == 'X' || strToDec[0] == 'x') && (strToDec[1] == 'n' || strToDec[1] == 'N') && strToDec[2] == '-' && strToDec[3] == '-')
		{
			strToDec += 4;
			const UTF8Char *sptr;
			UTF8Char c;

			UOSInt n = 128;//initial_n
			UOSInt i = 0;
			UOSInt bias = 72;//initial_bias

			UOSInt destSize = 0;
			UOSInt lastMinus = 0;
			sptr = strToDec;
			while (true)
			{
				c = *sptr;
				if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
				{
					sptr++;
				}
				else if (c == '-')
				{
					lastMinus = (UOSInt)(sptr - strToDec);
					sptr++;
				}
				else
					break;
			}

			if (lastMinus > 0)
			{
				sptr = &strToDec[lastMinus];
				while (strToDec < sptr)
				{
					buff[destSize] = *strToDec++;
					destSize++;
				}
				strToDec = sptr + 1;
			}
			UOSInt digit;
			UOSInt t;
			UOSInt out = destSize;
			while (true)
			{
				c = *strToDec;
				if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9'))
				{
					break;
				}
				UOSInt oldi = i;
				UOSInt w = 1;
				UOSInt k = BASE; //base;
				while (true)
				{
					c = *strToDec;
					if (c >= 'a' && c <= 'z')
					{
						digit = (UOSInt)(c - 'a');
					}
					else if (c >= 'A' && c <= 'Z')
					{
						digit = (UOSInt)(c - 'A');
					}
					else if (c >= '0' && c <= '9')
					{
						digit = (UOSInt)(c - '0' + 26);
					}
					else
					{
						break;
					}
					strToDec++;

					i += digit * w;
					t = (k <= bias)?TMIN:((k >= bias + TMAX)?TMAX:k - bias);
					if (digit < t)
						break;
					w = w * (BASE - t);
					k += BASE; //base;
				}

				bias = Adapt(i - oldi, out + 1, oldi == 0);
				n += i / (out + 1);
				i %= (out + 1);

				MemCopyO(&buff[i + 1], &buff[i], (destSize - i) * sizeof(UTF8Char));
				////////////////////////////////
				if (n < 0x80)
				{
					buff[i] = (UTF8Char)n;
					destSize++;
					i++;
					out++;
				}
				else if (n < 0x800)
				{
					buff[i] = (UTF8Char)(0xc0 | (n >> 6));
					buff[i + 1] = (UTF8Char)(0x80 | (n & 0x3f));
					destSize += 2;
					i += 2;
					out += 2;
				}
				else if (n < 0x10000)
				{
					buff[i] = (UTF8Char)(0xe0 | (n >> 12));
					buff[i + 1] = (UTF8Char)(0x80 | ((n >> 6) & 0x3f));
					buff[i + 2] = (UTF8Char)(0x80 | (n & 0x3f));
					destSize += 3;
					i += 3;
					out += 3;
				}
				else if (n < 0x200000)
				{
					buff[i] = (UTF8Char)(0xf0 | (n >> 18));
					buff[i + 1] = (UTF8Char)(0x80 | ((n >> 12) & 0x3f));
					buff[i + 2] = (UTF8Char)(0x80 | ((n >> 6) & 0x3f));
					buff[i + 3] = (UTF8Char)(0x80 | (n & 0x3f));
					destSize += 4;
					i += 4;
					out += 4;
				}
				else if (n < 0x4000000)
				{
					buff[i] = (UTF8Char)(0xf8 | (n >> 24));
					buff[i + 1] = (UTF8Char)(0x80 | ((n >> 18) & 0x3f));
					buff[i + 2] = (UTF8Char)(0x80 | ((n >> 12) & 0x3f));
					buff[i + 3] = (UTF8Char)(0x80 | ((n >> 6) & 0x3f));
					buff[i + 4] = (UTF8Char)(0x80 | (n & 0x3f));
					destSize += 5;
					i += 5;
					out += 5;
				}
				else
				{
					buff[i] = (UTF8Char)(0xfc | (n >> 30));
					buff[i + 1] = (UTF8Char)(0x80 | ((n >> 24) & 0x3f));
					buff[i + 2] = (UTF8Char)(0x80 | ((n >> 18) & 0x3f));
					buff[i + 3] = (UTF8Char)(0x80 | ((n >> 12) & 0x3f));
					buff[i + 4] = (UTF8Char)(0x80 | ((n >> 6) & 0x3f));
					buff[i + 5] = (UTF8Char)(0x80 | (n & 0x3f));
					destSize += 6;
					i += 6;
					out += 6;
				}
			}
			buff += destSize;
		}
		else
		{
			*buff++ = *strToDec++;
		}
	}
	*buff = 0;
	return buff;
}

WChar *Text::TextEnc::Punycode::Decode(WChar *buff, const UTF8Char *strToDec)
{
	while (*strToDec)
	{
		if ((strToDec[0] == 'X' || strToDec[0] == 'x') && (strToDec[1] == 'n' || strToDec[1] == 'N') && strToDec[2] == '-' && strToDec[3] == '-')
		{
			strToDec += 4;
			const UTF8Char *sptr;
			UTF8Char c;

			UOSInt n = 128;//initial_n
			UOSInt i = 0;
			UOSInt bias = 72;//initial_bias

			UOSInt destSize = 0;
			UOSInt lastMinus = 0;
			sptr = strToDec;
			while (true)
			{
				c = *sptr;
				if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
				{
					sptr++;
				}
				else if (c == '-')
				{
					lastMinus = (UOSInt)(sptr - strToDec);
					sptr++;
				}
				else
					break;
			}

			if (lastMinus > 0)
			{
				sptr = &strToDec[lastMinus];
				while (strToDec < sptr)
				{
					buff[destSize] = *strToDec++;
					destSize++;
				}
				strToDec = sptr + 1;
			}
			UOSInt digit;
			UOSInt t;
			UOSInt out = destSize;
			while (true)
			{
				c = *strToDec;
				if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9'))
				{
					break;
				}
				UOSInt oldi = i;
				UOSInt w = 1;
				UOSInt k = BASE; //base;
				while (true)
				{
					c = *strToDec;
					if (c >= 'a' && c <= 'z')
					{
						digit = (UOSInt)(c - 'a');
					}
					else if (c >= 'A' && c <= 'Z')
					{
						digit = (UOSInt)(c - 'A');
					}
					else if (c >= '0' && c <= '9')
					{
						digit = (UOSInt)(c - '0' + 26);
					}
					else
					{
						break;
					}
					strToDec++;

					i += digit * w;
					t = (k <= bias)?TMIN:((k >= bias + TMAX)?TMAX:k - bias);
					if (digit < t)
						break;
					w = w * (BASE - t);
					k += BASE; //base;
				}

				bias = Adapt(i - oldi, out + 1, oldi == 0);
				n += i / (out + 1);
				i %= (out + 1);

				MemCopyO(&buff[i + 1], &buff[i], (destSize - i) * sizeof(WChar));
				buff[i] = (WChar)n;
				destSize++;
				i++;
				out++;
			}
			buff += destSize;
		}
		else
		{
			*buff++ = *strToDec++;
		}
	}
	*buff = 0;
	return buff;
}

WChar *Text::TextEnc::Punycode::Decode(WChar *buff, const WChar *strToDec)
{
	while (*strToDec)
	{
		if ((strToDec[0] == 'X' || strToDec[0] == 'x') && (strToDec[1] == 'n' || strToDec[1] == 'N') && strToDec[2] == '-' && strToDec[3] == '-')
		{
			strToDec += 4;
			const WChar *sptr;
			WChar c;

			UOSInt n = 128;//initial_n
			UOSInt i = 0;
			UOSInt bias = 72;//initial_bias

			UOSInt destSize = 0;
			UOSInt lastMinus = 0;
			sptr = strToDec;
			while (true)
			{
				c = *sptr;
				if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
				{
					sptr++;
				}
				else if (c == '-')
				{
					lastMinus = (UOSInt)(sptr - strToDec);
					sptr++;
				}
				else
					break;
			}

			if (lastMinus > 0)
			{
				sptr = &strToDec[lastMinus];
				while (strToDec < sptr)
				{
					buff[destSize] = *strToDec++;
					destSize++;
				}
				strToDec = sptr + 1;
			}
			UOSInt digit;
			UOSInt t;
			UOSInt out = destSize;
			while (true)
			{
				c = *strToDec;
				if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9'))
				{
					break;
				}
				UOSInt oldi = i;
				UOSInt w = 1;
				UOSInt k = BASE; //base;
				while (true)
				{
					c = *strToDec;
					if (c >= 'a' && c <= 'z')
					{
						digit = (UOSInt)(c - 'a');
					}
					else if (c >= 'A' && c <= 'Z')
					{
						digit = (UOSInt)(c - 'A');
					}
					else if (c >= '0' && c <= '9')
					{
						digit = (UOSInt)(c - '0' + 26);
					}
					else
					{
						break;
					}
					strToDec++;

					i += digit * w;
					t = (k <= bias)?TMIN:((k >= bias + TMAX)?TMAX:k - bias);
					if (digit < t)
						break;
					w = w * (BASE - t);
					k += BASE; //base;
				}

				bias = Adapt(i - oldi, out + 1, oldi == 0);
				n += i / (out + 1);
				i %= (out + 1);

				MemCopyO(&buff[i + 1], &buff[i], (destSize - i) * sizeof(WChar));
				buff[i] = (WChar)n;
				destSize++;
				i++;
				out++;
			}
			buff += destSize;
		}
		else
		{
			*buff++ = *strToDec++;
		}
	}
	*buff = 0;
	return buff;
}

Text::TextEnc::Punycode::Punycode()
{
}

Text::TextEnc::Punycode::~Punycode()
{
}

UTF8Char *Text::TextEnc::Punycode::EncodeString(UTF8Char *buff, const WChar *strToEnc)
{
	return Encode(buff, strToEnc);
}

UTF8Char *Text::TextEnc::Punycode::EncodeString(UTF8Char *buff, const UTF8Char *strToEnc)
{
	return Encode(buff, strToEnc);
}

UTF8Char *Text::TextEnc::Punycode::DecodeString(UTF8Char *buff, const UTF8Char *strToDec)
{
	return Decode(buff, strToDec);
}
