#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "Text/MyString.h"
#include "Text/StringBuilder.h"
#include "Text/Cpp/CppEnv.h"
#include "Text/Cpp/CppCodeParser.h"

UTF8Char *Text::Cpp::CppCodeParser::RemoveSpace(UTF8Char *sptr)
{
	UTF8Char c;
	while ((c = *sptr++) != 0)
	{
		if (c != ' ' && c != '\t')
			break;
		if (c == '/' && *sptr == '*')
		{
		}
	}
	return sptr - 1;
}

void Text::Cpp::CppCodeParser::LogError(Text::Cpp::CppParseStatus *status, const UTF8Char *errMsg, Data::ArrayListStrUTF8 *errMsgs)
{
	Text::StringBuilderUTF8 sb;
	Text::Cpp::CppParseStatus::FileParseStatus *fileStatus = status->GetFileStatus();
	const UTF8Char *fname = fileStatus->fileName;
	OSInt i = Text::StrLastIndexOf(fname, '\\');
	sb.Append(&fname[i + 1]);
	sb.Append((const UTF8Char*)" (");
	sb.AppendI32(fileStatus->lineNum);
	sb.Append((const UTF8Char*)"): ");
	sb.Append(errMsg);
	errMsgs->Add(Text::StrCopyNew(sb.ToString()));
}

Bool Text::Cpp::CppCodeParser::ParseSharpIfParam(const UTF8Char *cond, Text::Cpp::CppParseStatus *status, Data::ArrayListStrUTF8 *errMsgs, Data::ArrayList<const UTF8Char *> *codePhases, UOSInt cpIndex)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UTF8Char c;
	Bool succ = true;
	if (Text::StrEquals(cond, (const UTF8Char*)"defined(_WIN32_WCE)"))
	{
		sptr = sbuff;
	}
	sptr = sbuff;
	while (succ)
	{
		c = *cond++;
		if (c == '_')
		{
			*sptr++ = c;
		}
		else if (c >= 'A' && c <= 'Z')
		{
			*sptr++ = c;
		}
		else if (c >= 'a' && c <= 'z')
		{
			*sptr++ = c;
		}
		else if (c >= '0' && c <= '9')
		{
			*sptr++ = c;
		}
		else
		{
			if (sptr != sbuff)
			{
				*sptr = 0;
				codePhases->Insert(cpIndex++, Text::StrCopyNew(sbuff));
				sptr = sbuff;
			}

			if (c == 0)
				break;
			if (c == 0x20 || c == '\t')
			{
			}
			else if (c == '!')
			{
				if (*cond == '=')
				{
					codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"!="));
					cond++;
				}
				else
				{
					codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"!"));
				}
			}
			else if (c == '>')
			{
				if (*cond == '=')
				{
					codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)">="));
					cond++;
				}
				else if (*cond == '>')
				{
					codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)">>"));
					cond++;
				}
				else
				{
					codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)">"));
				}
			}
			else if (c == '<')
			{
				if (*cond == '=')
				{
					codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"<="));
					cond++;
				}
				else if (*cond == '<')
				{
					codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"<<"));
					cond++;
				}
				else
				{
					codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"<"));
				}
			}
			else if (c == '=' && *cond == '=')
			{
				codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"=="));
				cond++;
			}
			else if (c == '(')
			{
				codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"("));
			}
			else if (c == ')')
			{
				codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)")"));
			}
			else if (c == '&')
			{
				if (*cond == '&')
				{
					codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"&&"));
					cond++;
				}
				else
				{
					codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"&"));
				}
			}
			else if (c == '|')
			{
				if (*cond == '|')
				{
					codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"||"));
					cond++;
				}
				else
				{
					codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"|"));
				}
			}
			else if (c == '*')
			{
				codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"*"));
			}
			else if (c == '/')
			{
				codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"/"));
			}
			else if (c == '%')
			{
				codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"%"));
			}
			else if (c == '+')
			{
				codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"+"));
			}
			else if (c == '-')
			{
				codePhases->Insert(cpIndex++, Text::StrCopyNew((const UTF8Char*)"-"));
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				sb.Append((const UTF8Char*)"Unknown symbol '");
				sb.AppendChar(c, 1);
				sb.Append((const UTF8Char*)"'");
				this->LogError(status, sb.ToString(), errMsgs);
				succ = false;
			}
		}
	}
	return succ;
}

Bool Text::Cpp::CppCodeParser::EvalSharpIfVal(Data::ArrayList<const UTF8Char *> *codePhases, Text::Cpp::CppParseStatus *status, Data::ArrayListStrUTF8 *errMsgs, UOSInt cpIndex, Int32 *outVal, OSInt priority)
{
/*
	Priority:
	14	! ~
	13	.* ->*
	12	* / %
	11	+ -
	10	>> <<
	9	>= <= > <
	8	== !=
	7	&
	6	^
	5	|
	4	&&
	3	||
	2	= += -= *= /= %= <<= >>= &= ^= |=
	1	,
*/

	Text::StringBuilderUTF8 debugSB;
	OSInt i;
	OSInt j;
	const UTF8Char *phase;
	i = 0;
	j = codePhases->GetCount();
	while (i < j)
	{
		debugSB.Append(codePhases->GetItem(i));
		debugSB.Append((const UTF8Char*)" ");
		i++;
	}

	Int32 val = 0;
	phase = codePhases->RemoveAt(cpIndex);
	if (phase == 0)
	{
		return false;
	}

	if (Text::StrEquals(phase, (const UTF8Char*)"defined"))
	{
		Text::StrDelNew(phase);
		if (cpIndex + 3 <= codePhases->GetCount())
		{
			if (Text::StrEquals(codePhases->GetItem(cpIndex), (const UTF8Char*)"(") && Text::StrEquals(codePhases->GetItem(cpIndex + 2), (const UTF8Char*)")"))
			{
				Text::StrDelNew(codePhases->RemoveAt(cpIndex + 2));
				Text::StrDelNew(codePhases->RemoveAt(cpIndex));
				phase = codePhases->RemoveAt(cpIndex);
				if (status->IsDefined(phase))
				{
					val = 1;
				}
				else
				{
					val = 0;
				}
				Text::StrDelNew(phase);
			}
			else
			{
				phase = codePhases->RemoveAt(cpIndex);
				if (status->IsDefined(phase))
				{
					val = 1;
				}
				else
				{
					val = 0;
				}
				Text::StrDelNew(phase);
			}
		}
		else if (cpIndex + 1 == codePhases->GetCount())
		{
			phase = codePhases->RemoveAt(cpIndex);
			if (status->IsDefined(phase))
			{
				val = 1;
			}
			else
			{
				val = 0;
			}
			Text::StrDelNew(phase);
		}
		else
		{
			debugSB.Append((const UTF8Char*)": invalid defined syntex");
			this->LogError(status, debugSB.ToString(), errMsgs);
			return false;
		}
	}
	else if (Text::StrEquals(phase, (const UTF8Char*)"__has_feature"))
	{
		Text::StrDelNew(phase);
		if (cpIndex + 3 <= codePhases->GetCount())
		{
			if (Text::StrEquals(codePhases->GetItem(cpIndex), (const UTF8Char*)"(") && Text::StrEquals(codePhases->GetItem(cpIndex + 2), (const UTF8Char*)")"))
			{
				Text::StrDelNew(codePhases->RemoveAt(cpIndex + 2));
				Text::StrDelNew(codePhases->RemoveAt(cpIndex));
				phase = codePhases->RemoveAt(cpIndex);
				val = 0;
				Text::StrDelNew(phase);
			}
			else
			{
				phase = codePhases->RemoveAt(cpIndex);
				val = 0;
				Text::StrDelNew(phase);
			}
		}
		else if (cpIndex + 1 == codePhases->GetCount())
		{
			phase = codePhases->RemoveAt(cpIndex);
			val = 0;
			Text::StrDelNew(phase);
		}
		else
		{
			debugSB.Append((const UTF8Char*)": invalid __has_feature syntex");
			this->LogError(status, debugSB.ToString(), errMsgs);
			return false;
		}
	}
	else if (Text::StrEquals(phase, (const UTF8Char*)"("))
	{
		Text::StrDelNew(phase);
		if (!EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &val, 0))
		{
			return false;
		}
		phase = codePhases->GetItem(cpIndex);
		if (phase == 0 || !Text::StrEquals(phase, (const UTF8Char*)")"))
		{
			debugSB.Append((const UTF8Char*)": missing )");
			if (phase)
			{
				debugSB.Append(phase);
			}
			this->LogError(status, debugSB.ToString(), errMsgs);
			return false;
		}
		Text::StrDelNew(codePhases->RemoveAt(cpIndex));
	}
	else if (Text::StrEquals(phase, (const UTF8Char*)"!"))
	{
		Text::StrDelNew(phase);
		Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &val, 14);
		if (!succ)
			return false;
		if (val != 0)
			val = 0;
		else
			val = 1;
	}
	else if (phase[0] >= '0' && phase[0] <= '9')
	{
		OSInt len = Text::StrCharCnt(phase);
		if (phase[len - 1] == 'L')
		{
			((UTF8Char*)phase)[len - 1] = 0;
		}
		if (!Text::StrToInt32(phase, &val))
		{
			debugSB.Append((const UTF8Char*)": unknown syntex ");
			debugSB.Append(phase);
			this->LogError(status, debugSB.ToString(), errMsgs);
			Text::StrDelNew(phase);
			return false;
		}
		Text::StrDelNew(phase);
	}
	else if (status->IsDefined(phase))
	{
		if (codePhases->GetCount() <= cpIndex || !Text::StrEquals(codePhases->GetItem(cpIndex), (const UTF8Char*)"("))
		{
			Text::StringBuilderUTF8 sb;
			status->GetDefineVal(phase, 0, &sb);
			Text::StrDelNew(phase);
			this->ParseSharpIfParam(sb.ToString(), status, errMsgs, codePhases, cpIndex);
			return this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, outVal, priority);
		}
		else
		{
			const UTF8Char *phase2;
			Int32 lev;
			Text::StringBuilderUTF8 params;
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			lev = 1;
			while (cpIndex < codePhases->GetCount())
			{
				phase2 = codePhases->GetItem(cpIndex);
				if (Text::StrEquals(phase2, (const UTF8Char*)")"))
				{
					Text::StrDelNew(codePhases->RemoveAt(cpIndex));
					if (--lev <= 0)
					{
						break;
					}
					params.Append((const UTF8Char*)")");
				}
				else if (Text::StrEquals(phase2, (const UTF8Char*)"("))
				{
					lev++;
					params.Append((const UTF8Char*)"(");
					Text::StrDelNew(codePhases->RemoveAt(cpIndex));
				}
				else
				{
					params.Append(phase2);
					Text::StrDelNew(codePhases->RemoveAt(cpIndex));
				}
			}
			if (lev != 0)
			{
				debugSB.Append((const UTF8Char*)": macro not supported ");
				debugSB.Append(phase);
				this->LogError(status, debugSB.ToString(), errMsgs);
				Text::StrDelNew(phase);
				return false;
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				status->GetDefineVal(phase, params.ToString(), &sb);
				Text::StrDelNew(phase);
				this->ParseSharpIfParam(sb.ToString(), status, errMsgs, codePhases, cpIndex);
				return this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, outVal, priority);
			}
		}
	}
	else
	{
		debugSB.Append((const UTF8Char*)": unknown syntex ");
		debugSB.Append(phase);
		this->LogError(status, debugSB.ToString(), errMsgs);
		Text::StrDelNew(phase);
//		return false;
		val = 0;
	}

	while (codePhases->GetCount() > cpIndex)
	{
		phase = codePhases->GetItem(cpIndex);
		if (Text::StrEquals(phase, (const UTF8Char*)")"))
		{
			break;
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)"*"))
		{
			if (priority > 12)
			{
				break;
			}
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			Int32 nextVal;
			Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 12);
			if (!succ)
				break;
			val = val * nextVal;
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)"/"))
		{
			if (priority > 12)
			{
				break;
			}
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			Int32 nextVal;
			Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 12);
			if (!succ)
				break;
			val = val / nextVal;
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)"%"))
		{
			if (priority > 12)
			{
				break;
			}
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			Int32 nextVal;
			Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 12);
			if (!succ)
				break;
			val = val % nextVal;
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)"+"))
		{
			if (priority > 11)
			{
				break;
			}
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			Int32 nextVal;
			Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 11);
			if (!succ)
				break;
			val = val + nextVal;
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)"-"))
		{
			if (priority > 11)
			{
				break;
			}
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			Int32 nextVal;
			Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 11);
			if (!succ)
				break;
			val = val - nextVal;
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)">>"))
		{
			if (priority > 10)
			{
				break;
			}
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			Int32 nextVal;
			Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 10);
			if (!succ)
				break;
			val = val >> nextVal;
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)"<<"))
		{
			if (priority > 10)
			{
				break;
			}
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			Int32 nextVal;
			Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 10);
			if (!succ)
				break;
			val = val << nextVal;
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)">="))
		{
			if (priority > 9)
			{
				break;
			}
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			Int32 nextVal;
			Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 9);
			if (!succ)
				break;
			if (val >= nextVal)
				val = 1;
			else
				val = 0;
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)"<="))
		{
			if (priority > 9)
			{
				break;
			}
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			Int32 nextVal;
			Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 9);
			if (!succ)
				break;
			if (val <= nextVal)
				val = 1;
			else
				val = 0;
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)">"))
		{
			if (priority > 9)
			{
				break;
			}
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			Int32 nextVal;
			Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 9);
			if (!succ)
				break;
			if (val > nextVal)
				val = 1;
			else
				val = 0;
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)"<"))
		{
			if (priority > 9)
			{
				break;
			}
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			Int32 nextVal;
			Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 9);
			if (!succ)
				break;
			if (val < nextVal)
				val = 1;
			else
				val = 0;
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)"=="))
		{
			if (priority > 8)
			{
				break;
			}
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			Int32 nextVal;
			Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 8);
			if (!succ)
				break;
			if (val == nextVal)
				val = 1;
			else
				val = 0;
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)"!="))
		{
			if (priority > 8)
			{
				break;
			}
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			Int32 nextVal;
			Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 8);
			if (!succ)
				break;
			if (val != nextVal)
				val = 1;
			else
				val = 0;
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)"&"))
		{
			if (priority > 7)
			{
				break;
			}
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			Int32 nextVal;
			Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 7);
			if (!succ)
				break;
			val = val & nextVal;
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)"&&"))
		{
			if (priority > 4)
			{
				break;
			}
			Text::StrDelNew(codePhases->RemoveAt(cpIndex));
			if (val == 0)
			{
				i = 0;
				while (cpIndex < codePhases->GetCount())
				{
					phase = codePhases->GetItem(i);
					if (Text::StrEquals(phase, (const UTF8Char*)"("))
					{
						i++;
						Text::StrDelNew(codePhases->RemoveAt(i));
					}
					else if (Text::StrEquals(phase, (const UTF8Char*)")"))
					{
						i--;
						if (i < 0)
							break;
						Text::StrDelNew(codePhases->RemoveAt(i));
					}
					else
					{
						Text::StrDelNew(codePhases->RemoveAt(i));
					}
				}
				if (i < 0)
				{
					break;
				}
			}
			else
			{
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 4);
				if (!succ)
					break;
				if (nextVal != 0)
					val = 1;
				else
					val = 0;
			}
		}
		else if (Text::StrEquals(phase, (const UTF8Char*)"||"))
		{
			if (priority > 3)
			{
				break;
			}
			if (val != 0)
			{
				i = 0;
				while (cpIndex < codePhases->GetCount())
				{
					phase = codePhases->GetItem(i);
					if (Text::StrEquals(phase, (const UTF8Char*)"("))
					{
						i++;
						Text::StrDelNew(codePhases->RemoveAt(i));
					}
					else if (Text::StrEquals(phase, (const UTF8Char*)")"))
					{
						i--;
						if (i < 0)
							break;
						Text::StrDelNew(codePhases->RemoveAt(i));
					}
					else
					{
						Text::StrDelNew(codePhases->RemoveAt(i));
					}
				}
				if (i < 0)
				{
					break;
				}
			}
			else
			{
				Text::StrDelNew(codePhases->RemoveAt(cpIndex));
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 3);
				if (!succ)
					break;
				if (nextVal != 0)
					val = 1;
				else
					val = 0;
			}
		}
		else
		{
			debugSB.Append((const UTF8Char*)": unknown syntex ");
			debugSB.Append(phase);
			this->LogError(status, debugSB.ToString(), errMsgs);
			return false;
		}
	}
	*outVal = val;
	return true;
}

Bool Text::Cpp::CppCodeParser::EvalSharpIf(const UTF8Char *cond, Text::Cpp::CppParseStatus *status, Data::ArrayListStrUTF8 *errMsgs, Bool *result)
{
	Bool succ = true;
	OSInt i;
	OSInt j;
	const UTF8Char *phase;
	Data::ArrayList<const UTF8Char *> codePhase;
	if (!ParseSharpIfParam(cond, status, errMsgs, &codePhase, 0))
	{
		succ = false;
	}

	if (!succ)
	{
		i = 0;
		j = codePhase.GetCount();
		while (i < j)
		{
			phase = codePhase.GetItem(i);
			Text::StrDelNew(phase);
			i++;
		}
	}
	else
	{
		j = codePhase.GetCount();
		if (j > 0)
		{
			Int32 val;
			if (!EvalSharpIfVal(&codePhase, status, errMsgs, 0, &val, 0))
			{
				succ = false;
			}
			else
			{
				*result = (val != 0);
			}
			i = 0;
			j = codePhase.GetCount();
			if (succ && j > 0)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(cond);
				sb.Append((const UTF8Char*)": unknown phases found:");
				while (i < j)
				{
					sb.Append((const UTF8Char*)" ");
					sb.Append(codePhase.GetItem(i));
					i++;
				}
			}

			while (i < j)
			{
				phase = codePhase.GetItem(i);
				Text::StrDelNew(phase);
				i++;
			}
		}
		else
		{
			this->LogError(status, (const UTF8Char*)"#if empty condition", errMsgs);
			succ = false;
		}
	}
	return succ;
}

Bool Text::Cpp::CppCodeParser::ParseLine(UTF8Char *lineBuff, Text::Cpp::CppParseStatus *status, Data::ArrayListStrUTF8 *errMsgs)
{
	Bool lineStart;
	Bool nextLine = false;
	Bool parseStatus = true;
	UTF8Char c;
	UTF8Char *wordStart = 0;
	UTF8Char *sptr;
	UTF8Char sbuff[512];
	UTF8Char u8buff[512];
	Bool valid;
	OSInt i;
	OSInt j;
	Text::Cpp::CppParseStatus::FileParseStatus *fileStatus = status->GetFileStatus();

	fileStatus->lineNum++;
	if (fileStatus->lineNum == 290 && Text::StrEndsWithICase(fileStatus->fileName, (const UTF8Char*)"winspool.h"))
	{
		sptr = lineBuff;
	}

	i = fileStatus->lineBuffSB->GetLength();
	if (i > 0)
	{
		if (fileStatus->lineBuffWS)
		{
			j = fileStatus->lineBuffWS - fileStatus->lineBuffSB->ToString();
			fileStatus->lineBuffSB->Append(lineBuff);
			wordStart = fileStatus->lineBuffSB->ToString() + j;
		}
		else
		{
			fileStatus->lineBuffSB->Append(lineBuff);
		}
	}
	else
	{
		fileStatus->lineBuffSB->Append(lineBuff);
	}
	sptr = fileStatus->lineBuffSB->ToString() + i;
	if (fileStatus->currMode == Text::Cpp::CppParseStatus::PM_DEFINE)
	{
		lineStart = false;
	}
	else
	{
		lineStart = true;
	}
	if (*sptr == 0 && lineStart)
		nextLine = true;
	while (!nextLine)
	{
		valid = true;
		i = fileStatus->ifValid->GetCount();
		while (i-- > 0)
		{
			if (fileStatus->ifValid->GetItem(i) != 1)
			{
				valid = false;
				break;
			}
		}
		switch (fileStatus->currMode)
		{
		case Text::Cpp::CppParseStatus::PM_COMMENTPARA:
			while ((c = *sptr++) != 0)
			{
				sptr[-1] = ' ';
				if (c == '*' && *sptr == '/')
				{
					*sptr = ' ';
					OSInt i = fileStatus->pastModes->GetCount();
					if (i > 0)
					{
						fileStatus->currMode = fileStatus->pastModes->RemoveAt(i - 1);
					}
					else
					{
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					}
					sptr++;
					break;
				}
			}
			if (c == 0)
			{
				nextLine = true;
			}
			break;
		case Text::Cpp::CppParseStatus::PM_SHARP:
			lineStart = false;
			while (true)
			{
				c = *sptr++;
				if (wordStart)
				{
					if (c >= 'A' && c <= 'Z')
						continue;
					if (c >= 'a' && c <= 'z')
						continue;

					sptr[-1] = 0;
					if (Text::StrCompare(wordStart, (const UTF8Char*)"include") == 0)
					{
						if (valid)
						{
							sptr[-1] = c;
							if (c == '\"')
							{
								wordStart = sptr;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_INCLUDEQUOTE;
								break;
							}
							else if (c == '<')
							{
								wordStart = sptr;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_INCLUDEARROW;
								break;
							}
							else if (c == '\t' || c == ' ')
							{
								wordStart = 0;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_INCLUDE;
								break;
							}
							else if (c == '/' && *sptr == '*')
							{
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_INCLUDE);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								sptr++;
								break;
							}
							else if (c == '/' && *sptr == '/')
							{
								this->LogError(status, (const UTF8Char*)"Missing parameter in #include", errMsgs);
								parseStatus = false;
								nextLine = true;
								break;
							}
							else
							{
								this->LogError(status, (const UTF8Char*)"Unknown parameter in #include", errMsgs);
								parseStatus = false;
								nextLine = true;
								break;
							}
						}
						else
						{
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
							break;
						}
					}
					else if (Text::StrCompare(wordStart, (const UTF8Char*)"pragma") == 0)
					{
						if (valid)
						{
							if (c == '/' && *sptr == '*')
							{
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_PRAGMA);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								break;
							}
							else if (c == '/' && *sptr == '/')
							{
								nextLine = true;
								break;
							}
							else if (c == ' ' || c == '\t')
							{
								sptr[-1] = c;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_PRAGMA;
								wordStart = 0;
								break;
							}
							else
							{
								this->LogError(status, (const UTF8Char*)"Unknown parameter in #pragma", errMsgs);
								parseStatus = false;
								nextLine = true;
								break;
							}
						}
						else
						{
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
							break;
						}
					}
					else if (Text::StrCompare(wordStart, (const UTF8Char*)"ifndef") == 0)
					{
						fileStatus->modeStatus = 0;
						if (c == '/' && *sptr == '*')
						{
							fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_IFNDEF);
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
							wordStart = 0;
							break;
						}
						else if (c == '/' && *sptr == '/')
						{
							this->LogError(status, (const UTF8Char*)"Unknown parameter in #ifndef", errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
						else if (c == ' ' || c == '\t')
						{
							sptr[-1] = c;
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_IFNDEF;
							wordStart = 0;
							break;
						}
						else
						{
							this->LogError(status, (const UTF8Char*)"Unknown parameter in #ifndef", errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
					}
					else if (Text::StrCompare(wordStart, (const UTF8Char*)"ifdef") == 0)
					{
						fileStatus->modeStatus = 0;
						if (c == '/' && *sptr == '*')
						{
							fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_IFDEF);
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
							wordStart = 0;
							break;
						}
						else if (c == '/' && *sptr == '/')
						{
							this->LogError(status, (const UTF8Char*)"Unknown parameter in #ifdef", errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
						else if (c == ' ' || c == '\t')
						{
							sptr[-1] = c;
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_IFDEF;
							wordStart = 0;
							break;
						}
						else
						{
							this->LogError(status, (const UTF8Char*)"Unknown parameter in #ifdef", errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
					}
					else if (Text::StrCompare(wordStart, (const UTF8Char*)"if") == 0)
					{
						fileStatus->modeStatus = 0;
						if (c == '/' && *sptr == '*')
						{
							fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_IF);
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
							wordStart = 0;
							break;
						}
						else if (c == '/' && *sptr == '/')
						{
							this->LogError(status, (const UTF8Char*)"Unknown parameter in #if", errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
						else if (c == ' ' || c == '\t' || c == '(')
						{
							sptr[-1] = c;
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_IF;
							wordStart = 0;
							break;
						}
						else
						{
							this->LogError(status, (const UTF8Char*)"Unknown parameter in #if", errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
					}
					else if (Text::StrCompare(wordStart, (const UTF8Char*)"elif") == 0)
					{
						fileStatus->modeStatus = 0;
						if (c == '/' && *sptr == '*')
						{
							fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_ELIF);
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
							wordStart = 0;
							break;
						}
						else if (c == '/' && *sptr == '/')
						{
							nextLine = true;
							break;
						}
						else if (c == ' ' || c == '\t' || c == '(')
						{
							sptr[-1] = c;
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_ELIF;
							wordStart = 0;
							break;
						}
						else
						{
							this->LogError(status, (const UTF8Char*)"Unknown parameter in #elif", errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
					}
					else if (Text::StrCompare(wordStart, (const UTF8Char*)"define") == 0)
					{
						fileStatus->modeStatus = 0;
						if (c == '/' && *sptr == '*')
						{
							fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_DEFINE);
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
							break;
						}
						else if (c == '/' && *sptr == '/')
						{
							nextLine = true;
							break;
						}
						else if (c == ' ' || c == '\t')
						{
							sptr[-1] = c;
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_DEFINE;
							wordStart = 0;
							break;
						}
						else
						{
							this->LogError(status, (const UTF8Char*)"Unknown parameter in #define", errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
					}
					else if (Text::StrCompare(wordStart, (const UTF8Char*)"undef") == 0)
					{
						if (valid)
						{
							fileStatus->modeStatus = 0;
							if (c == '/' && *sptr == '*')
							{
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_UNDEF);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								break;
							}
							else if (c == '/' && *sptr == '/')
							{
								nextLine = true;
								break;
							}
							else if (c == ' ' || c == '\t')
							{
								sptr[-1] = c;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_UNDEF;
								wordStart = 0;
								break;
							}
							else
							{
								this->LogError(status, (const UTF8Char*)"Unknown parameter in #undef", errMsgs);
								parseStatus = false;
								nextLine = true;
								break;
							}
						}
						else
						{
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
							break;
						}
					}
					else if (Text::StrCompare(wordStart, (const UTF8Char*)"else") == 0)
					{
						i = fileStatus->ifValid->GetCount();
						if (i == 0)
						{
							this->LogError(status, (const UTF8Char*)"Corresponding #if not found in #else", errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
						else
						{
							Int32 lastV = fileStatus->ifValid->GetItem(i - 1);
							if (lastV == 0)
							{
								fileStatus->ifValid->SetItem(i - 1, 1);
							}
							else if (lastV == 1)
							{
								fileStatus->ifValid->SetItem(i - 1, 0);
							}
							else
							{
								fileStatus->ifValid->SetItem(i - 1, 2);
							}
							if (c == '/' && *sptr == '*')
							{
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_SHARPEND);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								break;
							}
							else if (c == '/' && *sptr == '/')
							{
								nextLine = true;
								break;
							}
							else if (c == ' ' || c == '\t' || c == 0)
							{
								sptr[-1] = c;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
								wordStart = 0;
								if (c == 0)
								{
									nextLine = true;
								}
								break;
							}
							else
							{
								this->LogError(status, (const UTF8Char*)"Unknown parameter in #else", errMsgs);
								parseStatus = false;
								nextLine = true;
								break;
							}
						}
					}
					else if (Text::StrCompare(wordStart, (const UTF8Char*)"endif") == 0)
					{
						i = fileStatus->ifValid->GetCount();
						if (i == 0)
						{
							this->LogError(status, (const UTF8Char*)"Corresponding #if not found in #endif", errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
						else
						{
							fileStatus->ifValid->RemoveAt(i - 1);
							if (c == '/' && *sptr == '*')
							{
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_SHARPEND);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								break;
							}
							else if (c == '/' && *sptr == '/')
							{
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
								nextLine = true;
								break;
							}
							else if (c == ' ' || c == '\t' || c == 0)
							{
								sptr[-1] = c;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
								wordStart = 0;
								if (c == 0)
									nextLine = true;
								break;
							}
							else
							{
								this->LogError(status, (const UTF8Char*)"Unknown parameter in #endif", errMsgs);
								parseStatus = false;
								nextLine = true;
								break;
							}
						}
					}
					else if (Text::StrCompare(wordStart, (const UTF8Char*)"error") == 0)
					{
						if (valid)
						{
							if (c == '/' && *sptr == '*')
							{
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_ERROR);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								break;
							}
							else if (c == '/' && *sptr == '/')
							{
								nextLine = true;
								break;
							}
							else if (c == ' ' || c == '\t')
							{
								sptr[-1] = c;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_ERROR;
								wordStart = 0;
								break;
							}
							else
							{
								this->LogError(status, (const UTF8Char*)"Unknown parameter in #error", errMsgs);
								parseStatus = false;
								nextLine = true;
								break;
							}
						}
						else
						{
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
							break;
						}
					}
					else
					{
						this->LogError(status, (const UTF8Char*)"Unknown '#' type", errMsgs);
						sptr[-1] = c;
						parseStatus = false;
						nextLine = true;
						break;
					}
				}
				else
				{
					if (c == ' ' || c == '\t')
					{
					}
					else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
					{
						wordStart = sptr - 1;
					}
					else if (c == '/' && *sptr == '*')
					{
						fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_SHARP);
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
						break;
					}
					else if (c == '/' && *sptr == '/')
					{
						this->LogError(status, (const UTF8Char*)"Unknown '#' type", errMsgs);
						parseStatus = false;
						nextLine = true;
						break;
					}
					else
					{
						this->LogError(status, (const UTF8Char*)"Unknown '#' type", errMsgs);
						parseStatus = false;
						nextLine = true;
						break;
					}
				}
				if (c == 0)
				{
					nextLine = true;
					break;
				}
			}
			break;
		case Text::Cpp::CppParseStatus::PM_INCLUDE:
			while ((c = *sptr++) != 0)
			{
				if (c == ' ' || c == '\t')
				{
				}
				else if (c == '\"')
				{
					wordStart = sptr;
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_INCLUDEQUOTE;
					break;
				}
				else if (c == '<')
				{
					wordStart = sptr;
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_INCLUDEARROW;
					break;
				}
				else if (c == '/' && *sptr == '*')
				{
					fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_INCLUDE);
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
					break;
				}
				else
				{
					this->LogError(status, (const UTF8Char*)"Unknown #include parameter", errMsgs);
					parseStatus = false;
					nextLine = true;
					break;
				}
			}
			break;
		case Text::Cpp::CppParseStatus::PM_INCLUDEQUOTE:
			while ((c = *sptr++) != 0)
			{
				if (c == '\"')
				{
					sptr[-1] = 0;
					u8buff[0] = 0;
					this->env->GetIncludeFilePath(u8buff, wordStart, status->GetCurrCodeFile());
					sptr[-1] = c;

					if (u8buff[0] == 0)
					{
						parseStatus = false;
						sptr[-1] = 0;
						Text::StrConcat(Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"Include file "), wordStart), (const UTF8Char*)" not found");
						sptr[-1] = c;
						this->LogError(status, sbuff, errMsgs);
						nextLine = true;
						break;
					}
					else
					{
						if (ParseFile(u8buff, errMsgs, status))
						{
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
							break;
						}
						else
						{
							parseStatus = false;
							nextLine = true;
							break;
						}
					}
				}
				else
				{
				}
			}
			if (c == 0)
			{
				nextLine = true;
			}
			break;
		case Text::Cpp::CppParseStatus::PM_INCLUDEARROW:
			while ((c = *sptr++) != 0)
			{
				if (c == '>')
				{
					sptr[-1] = 0;
					this->env->GetIncludeFilePath(u8buff, wordStart, 0);
					sptr[-1] = c;

					if (u8buff[0] == 0)
					{
						parseStatus = false;
						Text::StrConcat(Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"Include file "), wordStart), (const UTF8Char*)" not found");
						this->LogError(status, sbuff, errMsgs);
					}
					else
					{
						if (ParseFile(u8buff, errMsgs, status))
						{
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
						}
						else
						{
							parseStatus = false;
							nextLine = true;
							break;
						}
					}
				}
				else
				{
				}
			}
			if (c == 0)
			{
				nextLine = true;
			}
			break;
		case Text::Cpp::CppParseStatus::PM_PRAGMA:
			while ((c = *sptr++) != 0)
			{
				if (c == ' ' || c == '\t')
				{
				}
				else if (c == '/' && *sptr == '/')
				{
					nextLine = true;
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					break;
				}
				else if (c == '/' && *sptr == '*')
				{
					fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_PRAGMA);
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
					break;
				}
				else
				{
					wordStart = sptr - 1;
				}
			}
			if (c == 0)
			{
				fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
				nextLine = true;
			}
			break;
		case Text::Cpp::CppParseStatus::PM_IFNDEF:
			/////////////////////////////////////////////
			while ((c = *sptr++) != 0)
			{
				if (c == ' ' || c == '\t')
				{
					if (wordStart)
					{
						sptr[-1] = 0;
						fileStatus->ifValid->Add((!status->IsDefined(wordStart))?1:0);
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
						break;
					}
				}
				else if (c == '/' && *sptr == '/')
				{
					nextLine = true;
					parseStatus = false;
					this->LogError(status, (const UTF8Char*)"Parameters not found in #ifndef", errMsgs);
					break;
				}
				else if (c == '/' && *sptr == '*')
				{
					fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_IFNDEF);
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
					break;
				}
				else
				{
					if (wordStart == 0)
					{
						wordStart = sptr - 1;
					}
				}
			}
			if (c == 0)
			{
				if (wordStart)
				{
					sptr[-1] = 0;
					fileStatus->ifValid->Add((!status->IsDefined(wordStart))?1:0);
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					nextLine = true;
					break;
				}
				else
				{
					nextLine = true;
					parseStatus = false;
					this->LogError(status, (const UTF8Char*)"Parameters not found in #ifndef", errMsgs);
					break;
				}
			}
			break;
		case Text::Cpp::CppParseStatus::PM_IFDEF:
			/////////////////////////////////////////////
			while ((c = *sptr++) != 0)
			{
				if (c == ' ' || c == '\t')
				{
					if (wordStart)
					{
						sptr[-1] = 0;
						fileStatus->ifValid->Add((status->IsDefined(wordStart))?1:0);
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
						break;
					}
				}
				else if (c == '/' && *sptr == '/')
				{
					nextLine = true;
					parseStatus = false;
					this->LogError(status, (const UTF8Char*)"Parameters not found in #ifdef", errMsgs);
					break;
				}
				else if (c == '/' && *sptr == '*')
				{
					fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_IFDEF);
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
					break;
				}
				else
				{
					if (wordStart == 0)
					{
						wordStart = sptr - 1;
					}
				}
			}
			if (c == 0)
			{
				if (wordStart)
				{
					sptr[-1] = 0;
					fileStatus->ifValid->Add((status->IsDefined(wordStart))?1:0);
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					nextLine = true;
					break;
				}
				else
				{
					nextLine = true;
					parseStatus = false;
					this->LogError(status, (const UTF8Char*)"Parameters not found in #ifdef", errMsgs);
					break;
				}
			}
			break;
		case Text::Cpp::CppParseStatus::PM_ELIF:
		case Text::Cpp::CppParseStatus::PM_IF:
			{
				/////////////////////////////////////////////
				Bool lastNextLine = false;
				while ((c = *sptr++) != 0)
				{
					if (c == ' ' || c == '\t')
					{
					}
					else if (c == '/' && *sptr == '/')
					{
						if (wordStart)
						{
							nextLine = true;
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
							c = 0;
							break;
						}
						else
						{
							nextLine = true;
							parseStatus = false;
							this->LogError(status, (const UTF8Char*)"Parameters not found in #if", errMsgs);
							break;
						}
					}
					else if (c == '/' && *sptr == '*')
					{
						fileStatus->pastModes->Add(fileStatus->currMode);
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
						sptr[-1] = ' ';
						*sptr = ' ';
						break;
					}
					else if (c == '\\')
					{
						lastNextLine = true;
						sptr[-1] = ' ';
					}
					else
					{
						lastNextLine = false;
						if (wordStart == 0)
						{
							wordStart = sptr - 1;
						}
					}
				}
				if (c == 0 && !lastNextLine)
				{
					if (wordStart)
					{
						sptr[-1] = 0;
						i = fileStatus->ifValid->GetCount();
						if (fileStatus->currMode == Text::Cpp::CppParseStatus::PM_ELIF)
						{
							if (i >= 2 && fileStatus->ifValid->GetItem(i - 2) == 0)
							{
							}
							else
							{
								Int32 lastV = fileStatus->ifValid->GetItem(i - 1);
								if (lastV == 0)
								{
									Bool ifRes;
									if (!EvalSharpIf(wordStart, status, errMsgs, &ifRes))
									{
										parseStatus = false;
										nextLine = true;
										break;
									}
									else
									{
										fileStatus->ifValid->SetItem(i - 1, ifRes?1:0);
									}
								}
								else
								{
									fileStatus->ifValid->SetItem(i - 1, 2);
								}
							}
						}
						else
						{
							if (i >= 1 && fileStatus->ifValid->GetItem(i - 1) == 0)
							{
								fileStatus->ifValid->Add(0);
							}
							else
							{
								Bool ifRes;
								if (!EvalSharpIf(wordStart, status, errMsgs, &ifRes))
								{
									parseStatus = false;
									nextLine = true;
									break;
								}
								else
								{
									fileStatus->ifValid->Add(ifRes?1:0);
								}
							}
						}
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
						nextLine = true;
						break;
					}
					else
					{
						nextLine = true;
						parseStatus = false;
						this->LogError(status, (const UTF8Char*)"Parameters not found in #if", errMsgs);
						break;
					}
				}
				else if (lastNextLine)
				{
					nextLine = true;
					break;
				}
			}
			break;
		case Text::Cpp::CppParseStatus::PM_DEFINE:
			{
				Bool lastNextLine = false;
				while ((c = *sptr++) != 0)
				{
					if (c == ' ' || c == '\t')
					{
					}
					else if (c == '/' && *sptr == '/')
					{
						nextLine = true;
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
						c = 0;
						sptr[-1] = 0;
						break;
					}
					else if (c == '/' && *sptr == '*')
					{
						fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_DEFINE);
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
						sptr[-1] = ' ';
						*sptr = ' ';
						break;
					}
					else if (c == '\\')
					{
						lastNextLine = true;
						sptr[-1] = ' ';
					}
					else
					{
						lastNextLine = false;
						if (wordStart == 0)
						{
							wordStart = sptr - 1;
						}

/*						if (fileStatus->modeStatus == 0)
						{
							if (wordStart == 0)
							{
								wordStart = sptr - 1;
							}
							else if (c == '(')
							{
								Bool found = false;
								UTF8Char *tmpPtr = wordStart;
								sptr[-1] = 0;
								while (true)
								{
									c = *tmpPtr++;
									if (c == ' ' || c == '\t')
									{
										found = true;
										break;
									}
									else if (c == 0)
									{
										break;
									}
								}
								if (found)
								{
									sptr[-1] = '(';
								}
								else
								{
									if (!valid)
									{
										sptr[-1] = c;
										fileStatus->modeStatus = 1;
									}
									else if (status->AddDef(wordStart, 0, fileStatus->lineNum))
									{
										sptr[-1] = c;
										fileStatus->modeStatus = 1;
									}
									else
									{
										parseStatus = false;
										this->LogError(status, L"Define already exist", errMsgs);
										nextLine = true;
										break;
									}
								}
							}
						}*/
					}
				}
				if (c == 0)
				{
					if (lastNextLine)
					{
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_DEFINE;
					}
					else if (fileStatus->modeStatus == 0 && wordStart)
					{
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
						if (!valid)
						{
							fileStatus->modeStatus = 1;
						}
						else
						{
							UTF8Char *paramPtr = 0;
							UTF8Char *tmpPtr = wordStart;
							Int32 mode = 0;
							/*
							  Name( Param ) Cont
							0011113355555577
							*/
							while (true)
							{
								c = *tmpPtr++;
								if (c == 0)
								{
									if (status->AddDef(wordStart, paramPtr, 0, fileStatus->lineNum))
									{
										fileStatus->modeStatus = 1;
									}
									else
									{
										parseStatus = false;
										this->LogError(status, (const UTF8Char*)"Define already exist", errMsgs);
										nextLine = true;
										break;
									}
									break;
								}
								else if (c == ' ' || c == '\t')
								{
									if (mode == 1)
									{
										mode = 7;
										tmpPtr[-1] = 0;
									}
								}
								else if (c == '('  && mode == 1)
								{
									mode = 3;
									tmpPtr[-1] = 0;
								}
								else if (c == ')')
								{
									if (mode == 5)
									{
										OSInt tmpI = -1;
										while (true)
										{
											tmpI--;
											c = tmpPtr[tmpI];
											if (c == ' ' || c == '\t')
											{
											}
											else
											{
												tmpPtr[tmpI + 1] = 0;
												break;
											}
										}
										mode = 7;
									}
									else if (mode == 3)
									{
										mode = 7;
 									}
									else
									{
										parseStatus = false;
										this->LogError(status, (const UTF8Char*)"Syntax error in define", errMsgs);
										nextLine = true;
										break;
									}
								}
								else
								{
									if (mode == 0)
									{
										mode = 1;
									}
									else if (mode == 3)
									{
										paramPtr = &tmpPtr[-1];
										mode = 5;
									}
									else if (mode == 2 || mode == 7)
									{
										if (status->AddDef(wordStart, paramPtr, &tmpPtr[-1], fileStatus->lineNum))
										{
											fileStatus->modeStatus = 1;
										}
										else
										{
											parseStatus = false;
											this->LogError(status, (const UTF8Char*)"Define already exist", errMsgs);
											nextLine = true;
											break;
										}
										break;
									}
								}
							}
						}
					}
					else if (!valid)
					{
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					}
					else if (fileStatus->modeStatus == 0)
					{
						parseStatus = false;
						this->LogError(status, (const UTF8Char*)"No parameters in #define", errMsgs);
					}
					else
					{
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					}
					nextLine = true;
				}
				break;
			}
		case Text::Cpp::CppParseStatus::PM_UNDEF:
			if (valid)
			{
				wordStart = 0;
				/////////////////////////////////////////////
				while ((c = *sptr++) != 0)
				{
					if (c == ' ' || c == '\t')
					{
						if (wordStart)
						{
							sptr[-1] = 0;
							if (status->Undefine(wordStart))
							{
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
								break;
							}
							else
							{
/*								this->LogError(status, L"#undef error, define not found", errMsgs);
								nextLine = true;
								parseStatus = false;*/
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
								break;
							}
							break;
						}
					}
					else if (c == '/' && *sptr == '/')
					{
						sptr[-1] = 0;

						if (status->Undefine(wordStart))
						{
							nextLine = true;
							break;
						}
						else
						{
/*							this->LogError(status, L"#undef error, define not found", errMsgs);
							nextLine = true;
							parseStatus = false;*/
							nextLine = true;
							break;
						}
						break;
					}
					else if (c == '/' && *sptr == '*')
					{
						if (wordStart)
						{
							sptr[-1] = 0;

							if (status->Undefine(wordStart))
							{
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_SHARPEND);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								break;
							}
							else
							{
								/*this->LogError(status, L"#undef error, define not found", errMsgs);
								nextLine = true;
								parseStatus = false;*/
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_SHARPEND);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								break;
							}
							sptr[-1] = c;
							break;
						}
						else
						{
							fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_UNDEF);
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
							break;
						}
					}
					else
					{
						if (wordStart == 0)
						{
							wordStart = sptr - 1;
						}
					}
				}
				if (c == 0)
				{
					if (wordStart)
					{
						if (status->Undefine(wordStart))
						{
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
							nextLine = true;
						}
						else
						{
							/*this->LogError(status, (const UTF8Char*)"#undef error, define not found", errMsgs);
							nextLine = true;
							parseStatus = false;*/
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
							nextLine = true;
						}
					}
					else
					{
						this->LogError(status, (const UTF8Char*)"#undef parameter error", errMsgs);
						nextLine = true;
						parseStatus = false;
					}
				}
				break;
			}
		case Text::Cpp::CppParseStatus::PM_ERROR:
			if (valid)
			{
				/////////////////////////////////////////////
				while ((c = *sptr++) != 0)
				{
					if (c == ' ' || c == '\t')
					{
					}
					else if (c == '/' && *sptr == '/')
					{
						nextLine = true;
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
						break;
					}
					else if (c == '/' && *sptr == '*')
					{
						fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_ERROR);
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
						break;
					}
					else
					{
						wordStart = sptr - 1;
					}
				}
				if (c == 0)
				{
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					nextLine = true;
				}
				break;
			}
		case Text::Cpp::CppParseStatus::PM_SHARPEND:
			if (valid)
			{
				while ((c = *sptr++) != 0)
				{
					if (c == ' ' || c == '\t')
					{
					}
					else if (c == '/' && *sptr == '/')
					{
						nextLine = true;
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
						break;
					}
					else if (c == '/' && *sptr == '*')
					{
						fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_SHARPEND);
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
						break;
					}
					else
					{
						nextLine = true;
						parseStatus = false;
						break;
					}
				}
				if (c == 0)
				{
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					nextLine = true;
				}
				break;
			}
		case Text::Cpp::CppParseStatus::PM_NORMAL:
			while ((c = *sptr++) != 0)
			{
				if (c == ' ' || c == '\t')
				{
					if (wordStart)
					{
						wordStart = 0;
					}
				}
				else if (c == '/' && *sptr == '*')
				{
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
					sptr++;
					break;
				}
				else if (c == '/' && *sptr == '/')
				{
					nextLine = true;
					break;
				}
				else if (c == '"')
				{
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_QUOTE;
					break;
				}
				else if (c == '\'')
				{
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SQUOTE;
					break;
				}
				else if (c == '#')
				{
					if (lineStart)
					{
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARP;
					}
					else
					{
						this->LogError(status, (const UTF8Char*)"'#' is not at beginning of line", errMsgs);
						nextLine = true;
					}
					break;
				}
				else
				{
					if (wordStart == 0)
					{
						wordStart = sptr - 1;
					}
				}
			}
			if (c == 0)
			{
				nextLine = true;
			}
			break;
		case Text::Cpp::CppParseStatus::PM_QUOTE:
			while ((c = *sptr++) != 0)
			{
				if (c == '\\')
				{
					sptr++;
				}
				else if (c == '"')
				{
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					break;
				}
			}
			if (c == 0)
			{
				nextLine = true;
			}
			break;
		case Text::Cpp::CppParseStatus::PM_SQUOTE:
			while ((c = *sptr++) != 0)
			{
				if (c == '\\')
				{
					sptr++;
				}
				else if (c == '\'')
				{
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					break;
				}
			}
			if (c == 0)
			{
				nextLine = true;
			}
			break;
		default:
			this->LogError(status, (const UTF8Char*)"Unknown status", errMsgs);
			parseStatus = false;
			nextLine = true;
			break;
		}
	}
	if (fileStatus->currMode == Text::Cpp::CppParseStatus::PM_SHARPEND)
	{
		fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
	}
	else if (parseStatus && fileStatus->currMode != Text::Cpp::CppParseStatus::PM_NORMAL && fileStatus->currMode != Text::Cpp::CppParseStatus::PM_COMMENTPARA && fileStatus->currMode != Text::Cpp::CppParseStatus::PM_DEFINE && fileStatus->currMode != Text::Cpp::CppParseStatus::PM_IF && fileStatus->currMode != Text::Cpp::CppParseStatus::PM_ELIF)
	{
		this->LogError(status, (const UTF8Char*)"Status error in line end", errMsgs);
		parseStatus = false;
	}
	if (fileStatus->currMode == Text::Cpp::CppParseStatus::PM_NORMAL)
	{
		fileStatus->lineBuffSB->ClearStr();
	}
	else
	{
		fileStatus->lineBuffWS = wordStart;
	}
	if (!parseStatus)
	{
		return false;
	}
	return parseStatus;

	/*

	///////////////////////////////////////////////////
	if (status->parseMode == PM_COMMENTPARA)
	{
	}
	////////////////////////////////////////////////////

	sptr = RemoveSpace(sptr);
	c = *sptr;
	if (c == 0)
		return true;
	if (c == '#')
	{
		sptr = RemoveSpace(sptr + 1);
		if (Text::StrStartsWith(sptr, L"include"))
		{
			sptr = RemoveSpace(sptr + 7);
			c = *sptr++;
			if (c == '<')
			{
				i = Text::StrIndexOf(sptr, L">");
				if (i < 0)
					return false;
				sptr[i] = 0;
				sptr = env->GetIncludeFilePath(sbuff, sptr);
				if (sptr)
				{
					return this->ParseFile(sbuff, errMsgs);
				}
				else
				{
					return false;
				}
			}
			else if (c == '"')
			{
				i = Text::StrIndexOf(sptr, L"\"");
				if (i < 0)
					return false;
				sptr[i] = 0;
				sptr = env->GetIncludeFilePath(sbuff, sptr);
				if (sptr)
				{
					return this->ParseFile(sbuff, errMsgs);
				}
				else
				{

					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else if (Text::StrStartsWith(sptr, L"pragma"))
		{
			///////////////////////////////////////////////////
			return true;
		}
		else if (Text::StrStartsWith(sptr, L"ifndef"))
		{
			if (sptr[6] != ' ' && sptr[6] != '\t')
				return false;
			///////////////////////////////////////////////////
			return true;
		}
		else if (Text::StrStartsWith(sptr, L"ifdef"))
		{
			if (sptr[5] != ' ' && sptr[5] != '\t')
				return false;
			sptr = RemoveSpace(sptr + 5);
			
			///////////////////////////////////////////////////
			return true;
		}
		else if (Text::StrStartsWith(sptr, L"if"))
		{
			///////////////////////////////////////////////////
			return true;
		}
		else if (Text::StrStartsWith(sptr, L"else"))
		{
			///////////////////////////////////////////////////
			return true;
		}
		else if (Text::StrStartsWith(sptr, L"elif"))
		{
			///////////////////////////////////////////////////
			return true;
		}
		else if (Text::StrStartsWith(sptr, L"endif"))
		{
			///////////////////////////////////////////////////
			return true;
		}
		else if (Text::StrStartsWith(sptr, L"define"))
		{
			if (sptr[6] != ' ' && sptr[6] != '\t')
				return false;
			sptr = RemoveSpace(sptr + 6);
			///////////////////////////////////////////////////
			return true;
		}
		else if (Text::StrStartsWith(sptr, L"undef"))
		{
			
			///////////////////////////////////////////////////
			return true;
		}
		else if (Text::StrStartsWith(sptr, L"error"))
		{
			///////////////////////////////////////////////////
			return true; //////////////////////////////
		}
		else
		{
			////////////////////////////////////////////////////
			return false;
		}
	}
	else
	{
		while (c = *sptr++)
		{
			switch (status->parseMode)
			{
			case PM_NORMAL:
				if (c == ' ' || c == '\t')
				{
					break;
				}
				else if (c == '/' && *sptr == '*')
				{
					status->parseMode = PM_COMMENTPARA;
					sptr++;
					break;
				}
				else if (c == '/' && *sptr == '/')
				{
					nextLine = true;
					break;
				}
				else if (c == '#')
				{
					status->parseMode = PM_SHARP;
					break;
				}
				else
				{
					parseStatus = false;
					break;
				}
			case PM_COMMENTPARA:
				if (c == '*' && *sptr == '/')
				{
					status->parseMode = PM_NORMAL;
					sptr++;
					break;
				}
				else
				{
					break;
				}
			case PM_SHARP:
				if (wordStart)
				{
					if (c >= 'A' && c <= 'Z')
						break;
					if (c >= 'a' && c <= 'z')
						break;
					if (c == ' ' || c == '\t' || c == '\"' || c == '<')
					{
						sptr[-1] = 0;
						if (Text::StrCompare(wordStart, L"include") == 0)
						{
							sptr[-1] = c;
							if (c == '\"')
							{
								wordStart = sptr;
								status->parseMode = PM_INCLUDEQUOTE;
								break;
							}
							else if (c == '<')
							{
								wordStart = sptr;
								status->parseMode = PM_INCLUDEARROW;
								break;
							}
							else
							{
								wordStart = 0;
								status->parseMode = PM_INCLUDE;
								break;
							}
						}
						else if (Text::StrCompare(wordStart, L"pragma") == 0)
						{
							sptr[-1] = c;
							status->parseMode = PM_PRAGMA;
							wordStart = 0;
							break;
						}
						else
						{
							sptr[-1] = c;
							parseStatus = false;
							break;
						}
					}
					else
					{
						parseStatus = false;
						break;
					}
				}
				else
				{
					if (c == ' ' || c == '\t')
					{
						break;
					}
					else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
					{
						wordStart = sptr - 1;
						break;
					}
					else
					{
						parseStatus = false;
						break;
					}
				}
			case PM_INCLUDE:
				if (c == ' ' || c == '\t')
				{
					break;
				}
				else if (c == '\"')
				{
					wordStart = sptr;
					status->parseMode = PM_INCLUDEQUOTE;
					break;
				}
				else if (c == '<')
				{
					wordStart = sptr;
					status->parseMode = PM_INCLUDEARROW;
					break;
				}
				else
				{
					parseStatus = false;
					break;
				}
			case PM_INCLUDEQUOTE:
				if (c == '\"')
				{
					sptr[-1] = 0;
					sbuff[0] = 0;
					this->env->GetIncludeFilePath(sbuff, wordStart);
					sptr[-1] = c;

					if (sbuff[0] == 0)
					{
						parseStatus = false;
						Text::StrConcat(Text::StrConcat(Text::StrConcat(sbuff, L"Include file "), wordStart), L" not found");
						errMsgs->Add(Text::StrCopyNew(sbuff));
					}
					else
					{
						if (ParseFile(sbuff, errMsgs))
						{
							status->parseMode = PM_SHARPEND;
						}
						else
						{
							parseStatus = false;
						}
					}
				}
				else
				{
					break;
				}
			case PM_INCLUDEARROW:
				if (c == '>')
				{
					sptr[-1] = 0;
					this->env->GetIncludeFilePath(sbuff, wordStart);
					sptr[-1] = c;

					if (sbuff[0] == 0)
					{
						parseStatus = false;
						Text::StrConcat(Text::StrConcat(Text::StrConcat(sbuff, L"Include file "), wordStart), L" not found");
						errMsgs->Add(Text::StrCopyNew(sbuff));
					}
					else
					{
						if (ParseFile(sbuff, errMsgs))
						{
							status->parseMode = PM_SHARPEND;
						}
						else
						{
							parseStatus = false;
						}
					}
				}
				else
				{
					break;
				}
			case PM_PRAGMA:
				if (wordStart)
					break;
				else if (c == ' ' || c == '\t')
				{
					break;
				}
				else
				{
					wordStart = sptr - 1;
				}
				break;
			default:
				parseStatus = false;
				break;
			}

			if (!parseStatus)
			{
				Text::StrConcat(Text::StrConcat(Text::StrInt32(Text::StrConcat(lineBuff, L"Error in parsing line "), status->lineNum), L" in "), status->fileName);
				errMsgs->Add(Text::StrCopyNew(lineBuff));
				break;
			}
			else if (nextLine)
			{
				break;
			}
		}
		if (status->parseMode == PM_PRAGMA)
		{
			if (wordStart)
			{
				status->parseMode = PM_NORMAL;
			}
		}
		
		if (status->parseMode != PM_NORMAL && status->parseMode != PM_COMMENTPARA && status->parseMode != PM_SHARPEND)
		{
			Text::StrConcat(Text::StrConcat(Text::StrInt32(Text::StrConcat(lineBuff, L"Error in parsing line "), status->lineNum), L" in "), status->fileName);
			errMsgs->Add(Text::StrCopyNew(lineBuff));
			return false;
		}
		else
		{
			return true;
		}
	}*/
}

Text::Cpp::CppCodeParser::CppCodeParser(Text::Cpp::CppEnv *env)
{
	this->env = env;
}

Text::Cpp::CppCodeParser::~CppCodeParser()
{
}

Bool Text::Cpp::CppCodeParser::ParseFile(const UTF8Char *fileName, Data::ArrayListStrUTF8 *errMsgs, Text::Cpp::CppParseStatus *status)
{
	UTF8Char *lineBuff;
	UTF8Char *sptr;
	OSInt i;
	IO::StreamReader *reader;
	IO::FileStream *fs;

	lineBuff = MemAlloc(UTF8Char, 65536);
	if (Text::StrEquals(fileName, (const UTF8Char*)"C:\\Program Files (x86)\\Microsoft Visual Studio .NET 2003\\Vc7\\include\\ctype.h"))
	{
		fs = 0;
	}

	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
	if (fs->IsError())
	{
		Text::Cpp::CppParseStatus::FileParseStatus *fileStatus = status->GetFileStatus();
		if (fileStatus)
		{
			i = Text::StrLastIndexOf(fileStatus->fileName, IO::Path::PATH_SEPERATOR);
			sptr = Text::StrConcat(lineBuff, &fileStatus->fileName[i + 1]);
			sptr = Text::StrConcat(sptr, (const UTF8Char*)" (");
			sptr = Text::StrOSInt(sptr, fileStatus->lineNum);
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"): ");
		}
		else
		{
			sptr = lineBuff;
		}
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"Cannot open \"");
		sptr = Text::StrConcat(sptr, fileName);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"\"");
		errMsgs->Add(Text::StrCopyNew(lineBuff));
		MemFree(lineBuff);
		DEL_CLASS(fs);
		return false;
	}

	status->BeginParseFile(fileName);
	Bool succ = true;

	NEW_CLASS(reader, IO::StreamReader(fs, 0));
	while (reader->ReadLine(lineBuff, 65535))
	{
		if (!ParseLine(lineBuff, status, errMsgs))
		{
			succ = false;
			break;
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);
	if (!status->EndParseFile(fileName))
	{
		succ = false;
		Text::StrConcat(Text::StrConcat(lineBuff, fileName), (const UTF8Char*)" File End error");
		errMsgs->Add(Text::StrCopyNew(lineBuff));
	}

	MemFree(lineBuff);
	return succ;
}

void Text::Cpp::CppCodeParser::FreeErrMsgs(Data::ArrayListStrUTF8 *errMsgs)
{
	OSInt i = errMsgs->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(errMsgs->RemoveAt(i));
	}
}
