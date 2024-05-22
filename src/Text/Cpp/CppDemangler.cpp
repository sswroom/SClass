#include "Stdafx.h"
#include "Text/Cpp/CppDemangler.h"

//#define PRINTDEBUG
//#define VERBOSE
#if defined(VERBOSE) || defined(PRINTDEBUG)
#include <stdio.h>
#endif

/*
Name mangling
https://itanium-cxx-abi.github.io/cxx-abi/abi.html#mangle.local-name

CPP Syntax:
v = void
w = wchar_t
b = bool
c = char
s = short
i = int
x = __int64
h = unsigned char
t = unsigned short
j = unsigned int
y = unsigned __int64
d = double
P = *
K = const
l = start of template
N = Start of namespace
I = Start of template
F = Start of Function
E = End of namespace/template
CnE = Constructor
DnE = Destructor
*/

void Text::Cpp::CppDemangler::AppendStr(NN<ParseEnv> env, UTF8Char *strStart)
{
	env->substr[env->strId].v = strStart;
	env->substr[env->strId].leng = (UOSInt)(env->sbuff - strStart);
#if defined(PRINTDEBUG)
	UTF8Char debugBuff[256];
	env->substr[env->strId].ConcatTo(debugBuff);
	printf("substr[%d] = \"%s\";\r\n", (UInt32)env->strId, debugBuff);
#endif
	env->strId++;
}

void Text::Cpp::CppDemangler::AppendNStr(NN<ParseEnv> env, UTF8Char *strStart)
{
	env->nstr[env->nstrId].v = strStart;
	env->nstr[env->nstrId].leng = (UOSInt)(env->sbuff - strStart);
#if defined(PRINTDEBUG)
	UTF8Char debugBuff[256];
	env->nstr[env->nstrId].ConcatTo(debugBuff);
	printf("nstr[%d] = \"%s\";\r\n", (UInt32)env->nstrId, debugBuff);
#endif
	env->nstrId++;
}

void Text::Cpp::CppDemangler::AppendTpl(NN<ParseEnv> env, UTF8Char *strStart)
{
	env->tplstr[env->tplId].v = strStart;
	env->tplstr[env->tplId].leng = (UOSInt)(env->sbuff - strStart);
#if defined(PRINTDEBUG)
	UTF8Char debugBuff[256];
	env->tplstr[env->tplId].ConcatTo(debugBuff);
	printf("tplstr[%d] = \"%s\";\r\n", (UInt32)env->tplId, debugBuff);
#endif
	env->tplId++;
}

Bool Text::Cpp::CppDemangler::ParseType(NN<ParseEnv> env, Bool firstPart)
{
	UOSInt ptrCnt = 0;
	Bool constVal = false;
	Bool refVal = false;
	UOSInt i;
	UTF8Char c;
	Bool foundName;
	UTF8Char *clsName = 0;
	UTF8Char *nStart;
	UTF8Char *nStart2;
	UTF8Char *nStart3;
	while (true)
	{
		c = env->funcName[0];
		if (c == 'P')
		{
			ptrCnt++;
			env->funcName++;
		}
		else if (c == 'K')
		{
			constVal = true;
			env->funcName++;
		}
		else if (c == 'R')
		{
			refVal = true;
			env->funcName++;
		}
		else
		{
			break;
		}
	}
	nStart = env->sbuff;
	nStart2 = env->sbuff;
	switch (env->funcName[0])
	{
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		if (constVal)
		{
			nStart2 = env->sbuff;
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart = env->sbuff;
		}
		else
		{
			nStart = env->sbuff;
		}
		c = *env->funcName++;
		i = (UOSInt)(c - '0');
		c = *env->funcName++;
		if (c >= '0' && c <= '9')
		{
			i = i * 10 + c - '0';
		}
		else
		{
			*env->sbuff++ = c;
			i--;
		}
		while (i-- > 0)
		{
			c = *env->funcName++;
			if (c == 0)
			{
#if defined(VERBOSE)
				printf("ELFFuncName: End of function before name end\r\n");
#endif
				env->valid = false;
				return false;
			}
			*env->sbuff++ = c;
		}
		break;
	case 'N':
		env->funcName++;
		if (env->funcName[0] == 'K')
		{
			env->constFunc = true;
			env->funcName++;
		}
		if (constVal)
		{
			nStart2 = env->sbuff;
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart = env->sbuff;
		}
		else
		{
			nStart = env->sbuff;
		}
		foundName = false;
		while (true)
		{
			c = *env->funcName++;
			switch (c)
			{
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				i = (UOSInt)(c - '0');
				if (foundName)
				{
					AppendNStr(env, nStart);
					AppendStr(env, clsName);
					env->sbuff[0] = ':';
					env->sbuff[1] = ':';
					env->sbuff += 2;
				}
				else
				{
					foundName = true;
				}
				clsName = env->sbuff;
				c = *env->funcName++;
				if (c >= '0' && c <= '9')
				{
					i = i * 10 + c - '0';
				}
				else
				{
					*env->sbuff++ = c;
					i--;
				}
				while (i-- > 0)
				{
					c = *env->funcName++;
					if (c == 0)
					{
#if defined(VERBOSE)
						printf("ELFFuncName: End of function before name end in N\r\n");
#endif
						env->valid = false;
						return false;
					}
					*env->sbuff++ = c;
				}
				break;
			case 'C':
				c = *env->funcName++;
				if (c == '0' || c == '1' || c == '2')
				{
					*env->sbuff = 0;
					AppendNStr(env, nStart);
					clsName = Text::StrConcat(env->sbuff + 2, clsName);
					env->sbuff[0] = ':';
					env->sbuff[1] = ':';
					i = Text::StrIndexOfChar(env->sbuff, '<');
					if (i != INVALID_INDEX)
					{
						env->sbuff[i] = 0;
						env->sbuff += i;
					}
					else
					{
						env->sbuff = clsName;
					}
					c = *env->funcName++;
					if (c != 'E')
					{
#if defined(VERBOSE)
						printf("ELFFuncName: Unexpected code '%c' after C\r\n", c);
#endif
						env->valid = false;
						return false;
					}
					AppendStr(env, nStart);
					return true;
				}
				else
				{
#if defined(VERBOSE)
					printf("ELFFuncName: Unexpected C code '%c'\r\n", c);
#endif
					env->valid = false;
					return false;
				}
				break;
			case 'D':
				c = *env->funcName++;
				if (c == '0' || c == '1' || c == '2')
				{
					*env->sbuff = 0;
					AppendNStr(env, nStart);
					clsName = Text::StrConcat(env->sbuff + 3, clsName);
					env->sbuff[0] = ':';
					env->sbuff[1] = ':';
					env->sbuff[2] = '~';
					i = Text::StrIndexOfChar(env->sbuff, '<');
					if (i != INVALID_INDEX)
					{
						env->sbuff[i] = 0;
						env->sbuff += i;
					}
					else
					{
						env->sbuff = clsName;
					}
					c = *env->funcName++;
					if (c != 'E')
					{
#if defined(VERBOSE)
						printf("ELFFuncName: Unexpected code '%c' after D\r\n", c);
#endif
						env->valid = false;
						return false;
					}
					AppendNStr(env, nStart);
					return true;
				}
				else
				{
#if defined(VERBOSE)
					printf("ELFFuncName: Unexpected D code '%c'\r\n", c);
#endif
					env->valid = false;
					return false;
				}
				break;
			case 'E':
				AppendNStr(env, nStart);
				while (ptrCnt-- > 0)
				{
					*env->sbuff++ = '*';
					AppendNStr(env, nStart);
				}
				if (constVal)
				{
					AppendNStr(env, nStart2);
				}
				return true;
			case 'I':
				AppendNStr(env, nStart);
				*env->sbuff++ = '<';
				nStart3 = env->sbuff;
				if (!ParseType(env, false))
				{
					return false;
				}
				if (firstPart)
				{
					AppendTpl(env, nStart3);
					AppendStr(env, nStart3);
				}
				while (true)
				{
					c = env->funcName[0];
					if (c == 'E')
					{
						env->funcName++;
						*env->sbuff++ = '>';
						AppendStr(env, nStart);
						break;
					}
					else if (c == 0)
					{
#if defined(VERBOSE)
						printf("ELFFuncName: End not found in I\r\n");
#endif
						env->valid = false;
						return false;
					}
					else
					{
						*env->sbuff++ = ',';
						*env->sbuff++ = ' ';
						nStart3 = env->sbuff;
						if (!ParseType(env, false))
						{
							return false;
						}
						if (firstPart)
						{
							AppendTpl(env, nStart3);
							AppendStr(env, nStart3);
						}
					}
				}
				break;
			case 'a':
				AppendNStr(env, nStart);
				c = *env->funcName++;
				if (c == 'n')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator&"));
				}
				else if (c == 'S')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator="));
				}
				else if (c == 'N')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator&="));
				}
				else if (c == 'a')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator&&"));
				}
				else
				{
#if defined(VERBOSE)
					printf("ELFFuncName: Unknown operator type '%c' in a\r\n", c);
#endif
					env->valid = false;
					return false;
				}
				c = *env->funcName++;
				if (c != 'E')
				{
#if defined(VERBOSE)
					printf("ELFFuncName: Unexpected code '%c' after a\r\n", c);
#endif
					env->valid = false;
					return false;
				}
				AppendStr(env, nStart);
				return true;
			case 'e':
				AppendNStr(env, nStart);
				c = *env->funcName++;
				if (c == 'q')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator=="));
				}
				else if (c == 'o')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator^"));
				}
				else if (c == 'O')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator^="));
				}
				else
				{
#if defined(VERBOSE)
					printf("ELFFuncName: Unknown operator type '%c' in e\r\n", c);
#endif
					env->valid = false;
					return false;
				}
				c = *env->funcName++;
				if (c != 'E')
				{
#if defined(VERBOSE)
					printf("ELFFuncName: Unexpected code '%c' after e\r\n", c);
#endif
					env->valid = false;
					return false;
				}
				AppendStr(env, nStart);
				return true;
			case 'g':
				AppendNStr(env, nStart);
				c = *env->funcName++;
				if (c == 't')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator>"));
				}
				else if (c == 'e')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator>="));
				}
				else
				{
#if defined(VERBOSE)
					printf("ELFFuncName: Unknown operator type '%c' in g\r\n", c);
#endif
					env->valid = false;
					return false;
				}
				c = *env->funcName++;
				if (c != 'E')
				{
#if defined(VERBOSE)
					printf("ELFFuncName: Unexpected code '%c' after g\r\n", c);
#endif
					env->valid = false;
					return false;
				}
				AppendStr(env, nStart);
				return true;
			case 'l':
				AppendNStr(env, nStart);
				c = *env->funcName++;
				if (c == 't')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator<"));
				}
				else if (c == 'e')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator<="));
				}
				else if (c == 's')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator<<"));
				}
				else if (c == 'S')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator<<="));
				}
				else
				{
#if defined(VERBOSE)
					printf("ELFFuncName: Unknown operator type '%c' in l\r\n", c);
#endif
					env->valid = false;
					return false;
				}
				c = *env->funcName++;
				if (c != 'E')
				{
#if defined(VERBOSE)
					printf("ELFFuncName: Unexpected code '%c' after l\r\n", c);
#endif
					env->valid = false;
					return false;
				}
				AppendStr(env, nStart);
				return true;
			case 'm':
				AppendNStr(env, nStart);
				c = *env->funcName++;
				if (c == 'i')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator-"));
				}
				else if (c == 'l')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator*"));
				}
				else if (c == 'm')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator--"));
				}
				else
				{
#if defined(VERBOSE)
					printf("ELFFuncName: Unknown operator type '%c' in m\r\n", c);
#endif
					env->valid = false;
					return false;
				}
				c = *env->funcName++;
				if (c != 'E')
				{
#if defined(VERBOSE)
					printf("ELFFuncName: Unexpected code '%c' after m\r\n", c);
#endif
					env->valid = false;
					return false;
				}
				AppendStr(env, nStart);
				return true;
			case 'p':
				AppendNStr(env, nStart);
				c = *env->funcName++;
				if (c == 'l')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator+"));
				}
				else if (c == 'L')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator+="));
				}
				else if (c == 'p')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator++"));
				}
				else if (c == 't')
				{
					env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("::operator->"));
				}
				else
				{
#if defined(VERBOSE)
					printf("ELFFuncName: Unknown operator type '%c' in p\r\n", c);
#endif
					env->valid = false;
					return false;
				}
				c = *env->funcName++;
				if (c != 'E')
				{
#if defined(VERBOSE)
					printf("ELFFuncName: Unexpected code '%c' after p\r\n", c);
#endif
					env->valid = false;
					return false;
				}
				AppendNStr(env, nStart);
				return true;
			case 'S':
				switch (env->funcName[0])
				{
				case '_':
					if (env->nstrId <= 0)
					{
#if defined(VERBOSE)
						printf("ELFFuncName: S_ occurs before first string\r\n");
#endif
						env->valid = false;
						return false;
					}
					env->sbuff = env->nstr[0].ConcatTo(env->sbuff);
					env->sbuff[0] = ':';
					env->sbuff[1] = ':';
					env->sbuff += 2;
					env->funcName++;
					break;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					i = (UOSInt)env->funcName[0] - '0' + 1;
					if (env->funcName[1] != '_' || env->nstrId <= i)
					{
#if defined(VERBOSE)
						printf("ELFFuncName: S%c_ occurs before string\r\n", c);
#endif
						env->valid = false;
						return false;
					}
					env->sbuff = env->nstr[i].ConcatTo(env->sbuff);
					env->sbuff[0] = ':';
					env->sbuff[1] = ':';
					env->sbuff += 2;
					env->funcName += 2;
					break;
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
					i = (UOSInt)env->funcName[0] - '7' + 1;
					if (env->funcName[1] != '_' || env->nstrId <= i)
					{
#if defined(VERBOSE)
						printf("ELFFuncName: S%c_ occurs before string\r\n", c);
#endif
						env->valid = false;
						return false;
					}
					env->sbuff = env->nstr[i].ConcatTo(env->sbuff);
					env->sbuff[0] = ':';
					env->sbuff[1] = ':';
					env->sbuff += 2;
					env->funcName += 2;
					break;
				default:
#if defined(VERBOSE)
					printf("ELFFuncName: Unexpected code '%c' after S\r\n", c);
#endif
					env->valid = false;
					return false;
				}
				break;
			default:
#if defined(VERBOSE)
				printf("ELFFuncName: Unexpected code '%c' after N\r\n", c);
#endif
				env->valid = false;
				return false;
			}
		}
		return false;
	case 'F':
		env->funcName++;
		nStart = env->sbuff;
		if (ptrCnt != 1)
		{
#if defined(VERBOSE)
			printf("ELFFuncName: Unexpected ptrCnt in F\r\n");
#endif
			env->valid = false;
			return false;
		}
		if (!ParseType(env, false))
		{
			return false;
		}
		*env->sbuff++ = ' ';
		*env->sbuff++ = '(';
		while (ptrCnt-- > 0)
		{
			*env->sbuff++ = '*';
		}
		*env->sbuff++ = ')';
		*env->sbuff++ = '(';
		if (!ParseType(env, false))
		{
			return false;
		}
		while (true)
		{
			c = env->funcName[0];
			if (c == 'E')
			{
				env->funcName++;
				*env->sbuff++ = ')';
				AppendStr(env, nStart);
				AppendStr(env, nStart);
				return true;
			}
			else if (c == 0)
			{
#if defined(VERBOSE)
				printf("ELFFuncName: End of code not found in F\r\n");
#endif
				env->valid = false;
				return false;
			}
			else
			{
				*env->sbuff++ = ',';
				*env->sbuff++ = ' ';
				if (!ParseType(env, false))
				{
					return false;
				}
			}
		}
	case 'v':
		env->funcName++;
		if (constVal)
		{
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart2 = nStart;
			nStart = env->sbuff;
		}
		env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("void"));
		break;
	case 'w':
		env->funcName++;
		if (constVal)
		{
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart2 = nStart;
			nStart = env->sbuff;
		}
		env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("wchar_t"));
		break;
	case 'b':
		env->funcName++;
		if (constVal)
		{
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart2 = nStart;
			nStart = env->sbuff;
		}
		env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("bool"));
		break;
	case 'a':
		env->funcName++;
		if (constVal)
		{
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart2 = nStart;
			nStart = env->sbuff;
		}
		env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("signed char"));
		break;
	case 'c':
		env->funcName++;
		if (constVal)
		{
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart2 = nStart;
			nStart = env->sbuff;
		}
		env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("char"));
		break;
	case 's':
		env->funcName++;
		if (constVal)
		{
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart2 = nStart;
			nStart = env->sbuff;
		}
		env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("short"));
		break;
	case 'i':
		env->funcName++;
		if (constVal)
		{
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart2 = nStart;
			nStart = env->sbuff;
		}
		env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("int"));
		break;
	case 'x':
		env->funcName++;
		if (constVal)
		{
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart2 = nStart;
			nStart = env->sbuff;
		}
		env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("__int64"));
		break;
	case 'h':
		env->funcName++;
		if (constVal)
		{
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart2 = nStart;
			nStart = env->sbuff;
		}
		env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("unsigned char"));
		break;
	case 't':
		env->funcName++;
		if (constVal)
		{
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart2 = nStart;
			nStart = env->sbuff;
		}
		env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("unsigned short"));
		break;
	case 'j':
		env->funcName++;
		if (constVal)
		{
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart2 = nStart;
			nStart = env->sbuff;
		}
		env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("unsigned int"));
		break;
	case 'm':
		env->funcName++;
		if (constVal)
		{
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart2 = nStart;
			nStart = env->sbuff;
		}
		env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("unsigned long"));
		break;
	case 'y':
		env->funcName++;
		if (constVal)
		{
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart2 = nStart;
			nStart = env->sbuff;
		}
		env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("unsigned __int64"));
		break;
	case 'd':
		if (firstPart)
		{
			if (env->funcName[1] == 'l')
			{
				env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("operator delete"));
				env->funcName += 2;
				return true;
			}
			else
			{
				env->valid = false;
				return false;
			}
		}
		else
		{
			env->funcName++;
			if (constVal)
			{
				env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
				nStart2 = nStart;
				nStart = env->sbuff;
			}
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("double"));
			break;
		}
	case 'f':
		env->funcName++;
		if (constVal)
		{
			env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
			nStart2 = nStart;
			nStart = env->sbuff;
		}
		env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("float"));
		break;
	case 'n':
		if (firstPart)
		{
			if (env->funcName[1] == 'w')
			{
				env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("operator new"));
				env->funcName += 2;
				return true;
			}
			else
			{
#if defined(VERBOSE)
				printf("ELFFuncName: Unknown code '%c' after n\r\n", c);
#endif
				env->valid = false;
				return false;
			}
		}
		else
		{
#if defined(VERBOSE)
			printf("ELFFuncName: n found in unexpected position\r\n");
#endif
			env->valid = false;
			return false;
		}
	case 'T':
		switch (env->funcName[1])
		{
		case '_':
			if (env->tplId <= 0)
			{
#if defined(VERBOSE)
				printf("ELFFuncName: T_ value not found\r\n");
#endif
				env->valid = false;
				return false;
			}
			if (constVal)
			{
				nStart2 = env->sbuff;
				env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
				nStart = env->sbuff;
			}
			env->sbuff = env->tplstr[0].ConcatTo(env->sbuff);
			env->funcName += 2;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			i = (UOSInt)env->funcName[1] - '0' + 1;
			if (env->funcName[2] != '_' || env->tplId <= i)
			{
#if defined(VERBOSE)
				printf("ELFFuncName: T%c_ value not found\r\n", c);
#endif
				env->valid = false;
				return false;
			}
			if (constVal)
			{
				nStart2 = env->sbuff;
				env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
				nStart = env->sbuff;
			}
			env->sbuff = env->tplstr[i].ConcatTo(env->sbuff);
			env->funcName += 3;
			AppendStr(env, nStart);
			break;
		default:
#if defined(VERBOSE)
			printf("ELFFuncName: Unknown code '%c' in T\r\n", c);
#endif
			env->valid = false;
			return false;
		}
		break;
	case 'S':
		switch (env->funcName[1])
		{
		case '_':
			if (env->strId <= 0)
			{
#if defined(VERBOSE)
				printf("ELFFuncName: String not found in S_\r\n");
#endif
				env->valid = false;
				return false;
			}
			if (constVal)
			{
				nStart2 = env->sbuff;
				env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
				nStart = env->sbuff;
			}
			env->sbuff = env->substr[0].ConcatTo(env->sbuff);
			env->funcName += 2;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			i = (UOSInt)env->funcName[1] - '0' + 1;
			if (env->funcName[2] != '_' || env->strId <= i)
			{
#if defined(VERBOSE)
				printf("ELFFuncName: String not found in S%c_\r\n", env->funcName[1]);
#endif
				env->valid = false;
				return false;
			}
			if (constVal)
			{
				nStart2 = env->sbuff;
				env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
				nStart = env->sbuff;
			}
			env->sbuff = env->substr[i].ConcatTo(env->sbuff);
			env->funcName += 3;
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			i = (UOSInt)env->funcName[1] - '7' + 1;
			if (env->funcName[2] != '_' || env->strId <= i)
			{
#if defined(VERBOSE)
				printf("ELFFuncName: String not found in S%c_\r\n", env->funcName[1]);
#endif
				env->valid = false;
				return false;
			}
			if (constVal)
			{
				nStart2 = env->sbuff;
				env->sbuff = Text::StrConcatC(env->sbuff, UTF8STRC("const "));
				nStart = env->sbuff;
			}
			env->sbuff = env->substr[i].ConcatTo(env->sbuff);
			env->funcName += 3;
			break;
		default:
#if defined(VERBOSE)
			printf("ELFFuncName: Unknown code '%c' in S\r\n", c);
#endif
			env->valid = false;
			return false;
		}
		break;
	default:
#if defined(VERBOSE)
		printf("ELFFuncName: Unknown code '%c'\r\n", c);
#endif
		env->valid = false;
		return false;
	}

	if (env->funcName[0] == 'I')
	{
		AppendStr(env, nStart);
		*env->sbuff++ = '<';
		env->funcName++;
		nStart3 = env->sbuff;
		if (!ParseType(env, false))
		{
			return false;
		}
		if (firstPart)
		{
			AppendTpl(env, nStart3);
			AppendStr(env, nStart3);
		}

		while (true)
		{
			c = env->funcName[0];
			if (c == 'E')
			{
				env->funcName++;
				*env->sbuff++ = '>';
				AppendStr(env, nStart);
				break;
			}
			else if (c == 0)
			{
#if defined(VERBOSE)
				printf("ELFFuncName: End of I not found\r\n");
#endif
				env->valid = false;
				return false;
			}
			else
			{
				*env->sbuff++ = ',';
				*env->sbuff++ = ' ';
				nStart3 = env->sbuff;
				if (!ParseType(env, false))
				{
					return false;
				}
				if (firstPart)
				{
					env->tplstr[env->tplId].v = nStart3;
					env->tplstr[env->tplId].leng = (UOSInt)(env->sbuff - nStart3);
					env->tplId++;
					AppendStr(env, nStart3);
				}
			}
		}
	}
	else
	{
		AppendStr(env, nStart);
	}
	if (refVal)
	{
		*env->sbuff++ = '&';
	}
	while (ptrCnt-- > 0)
	{
		*env->sbuff++ = '*';
		AppendStr(env, nStart);
	}
	if (constVal)
	{
		AppendStr(env, nStart2);
	}
	return true;
}


UTF8Char *Text::Cpp::CppDemangler::ToFuncName(UTF8Char *sbuff, const UTF8Char *funcName)
{
	if (funcName[0] == '_' && funcName[1] == 'Z')
	{
		ParseEnv env;
		env.funcName = funcName + 2;
		env.sbuff = sbuff;
		env.valid = true;
		env.constFunc = false;
		env.strId = 0;
		env.nstrId = 0;
		env.tplId = 0;
		if (env.funcName[0] == 'L')
		{
			env.sbuff = Text::StrConcatC(env.sbuff, UTF8STRC("static "));
			env.funcName++;
		}
		if (env.funcName[0] == 'T' && env.funcName[1] == 'h' && env.funcName[2] == 'n')
		{
			UTF8Char c;
			env.sbuff = Text::StrConcatC(env.sbuff, UTF8STRC("non-virtual thunk to "));
			env.funcName += 3;
			while (true)
			{
				c = env.funcName[0];
				if (c >= '0' && c <= '9')
				{
					env.funcName++;
				}
				else if (c == '_')
				{
					env.funcName++;
					break;
				}
				else
				{
					env.valid = false;
					break;
				}
			}
		}
		if (env.valid)
		{
			if (ParseType(env, true))
			{
				if (env.sbuff[-1] == '>' && env.funcName[0] == 'v')
				{
					MemCopyO(sbuff + 5, sbuff, (UOSInt)(env.sbuff - sbuff));
					env.sbuff += 5;
					MemCopyNO(sbuff, "void ", 5);
					env.funcName++;
					UOSInt i = env.tplId;
					while (i-- > 0)
					{
						env.tplstr[i].v += 5;
					}
					i = env.strId;
					while (i-- > 0)
					{
						env.substr[i].v += 5;
					}
				}
				*env.sbuff++ = '(';
			}
			else
			{
				env.valid = false;
			}
		}

		if (env.valid && ParseType(env, false))
		{
			while (*env.funcName && *env.funcName != '@' && env.valid)
			{
				*env.sbuff++ = ',';
				*env.sbuff++ = ' ';
				ParseType(env, false);
			}
		}

		if (env.valid)
		{
			*env.sbuff++ = ')';
			if (env.constFunc)
			{
				env.sbuff = Text::StrConcatC(env.sbuff, UTF8STRC(" const"));
			}
			else
			{
				*env.sbuff = 0;
			}
			return env.sbuff;
		}
		else
		{
			return Text::StrConcat(sbuff, funcName);
		}
	}
	else
	{
		return Text::StrConcat(sbuff, funcName);
	}
}
