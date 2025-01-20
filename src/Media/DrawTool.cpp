#include "Stdafx.h"
#include <windows.h>
#include <math.h>
#include "MyMemory.h"
#include "Sync/Event.h"
#include "Media\ImageResizer.h"
#include "Media\FrameInfo.h"
#include "Media\StaticImage.h"
#include "Media\DrawTool.h"

Media::DrawTool::DrawTool()
{
}

Media::DrawTool::~DrawTool()
{
}

void Media::DrawTool::DrawLineSimple(Media::StaticImage *img, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Int32 color)
{
	if ((color & 0xff000000) == 0)
		return;
	if (!img->To32bpp())
		return;
	Int32 *imgptr = (Int32*)img->data;
	Int32 imgWidth = img->info->width;
	Int32 imgHeight = img->info->height;
	Int32 lineW = imgWidth << 2;
	Int32 absX;
	Int32 absY;

	if (y1 > y2)
	{
		_asm
		{
			mov eax,x1
			mov ebx,x2
			mov ecx,y1
			mov edx,y2
			mov x2,eax
			mov x1,ebx
			mov y2,ecx
			mov y1,edx
		}
	}
	if ((color & 0xff000000) == 0xff000000)
	{
		_asm
		{
			mov eax,y2
			sub eax,y1
			mov absY,eax

			mov edx,x2
			sub edx,x1
			jnb dlslop
			neg edx
dlslop:
			mov absX,edx

			cmp eax,0
			jnz dlslop2
			cmp edx,0
			jz dlsexit
			
			mov ebx,y1
			cmp ebx,0
			jl dlsexit
			cmp ebx,imgHeight
			jge dlsexit

			mov eax,lineW
			mul ebx
			mov edi,imgptr
			add edi,eax
			
			mov eax,x1
			mov edx,x2
			cmp eax,edx
			jl dlshlop
			xchg eax,edx
dlshlop:
			cmp edx,0
			jle dlsexit
			cmp eax,imgWidth
			jge dlsexit

			cmp eax,0
			jge dlshlop2
			mov eax,0
dlshlop2:
			cmp edx,imgWidth
			jle dlshlop3
			mov edx,imgWidth
dlshlop3:
			sub edx,eax
			lea edi,[edi+eax*4]
			mov ecx,edx
			mov eax,color
			rep stosd
			jmp dlsexit

dlslop2:	// yDiff != 0
			cmp edx,0
			jnz dlslop3
			mov ebx,x1
			cmp ebx,0
			jl dlsexit
			cmp ebx,imgWidth
			jge dlsexit
			mov ecx,imgHeight
			mov eax,y1
			cmp eax,ecx
			jge dlsexit
			mov edx,y2
			cmp edx,ecx
			jle dlsvlop
			mov edx,ecx
dlsvlop:
			mov edi,imgptr
			lea edi,[edi+ebx*4]
			sub edx,eax
			mov ebx,lineW
			mov ecx,edx
			mul ebx
			add edi,eax
			mov eax,color
dlsvlop2:
			mov dword ptr [edi],eax
			add edi,ebx
			dec ecx
			jnz dlsvlop2
			jmp dlsexit
dlslop3:
			///////////////////////////////////////////////
			jmp dlsexit

dlsexit:
		}
	}
	else
	{
	}
}

/*
void Media::DrawTool::DrawLineSimple(Media::Image *img, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Int32 color)
{
	if ((color & 0xff000000) == 0)
		return;
	if (!img->To32bpp())
		return;
	Int32 *imgptr = (Int32*)img->data;
	Int32 imgWidth = img->info->width;
	Int32 imgHeight = img->info->height;
	Int32 lineW = imgWidth << 2;
	Int32 tmp;

	if (y1 > y2)
	{
		tmp = y1;
		y1 = y2;
		y2 = tmp;
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}

	Int32 xdiff = x2 - x1;
	Int32 ydiff = y2 - y1;
	if (xdiff == 0 && ydiff == 0)
		return;

	if ((color & 0xff000000) == 0xff000000)
	{
		if (ydiff == 0)
		{
			if (y1 < 0 || y1 >= imgHeight)
				return;
			Int32 stX;
			Int32 enX;
			if (x2 > x1)
			{
				stX = x1;
				enX = x2;
			}
			else
			{
				stX = x2;
				enX = x1;
			}
			if (stX > imgWidth)
				return;
			if (stX < 0)
				stX = 0;
			imgptr += y1 * imgWidth + stX;
#ifdef HAS_ASM32
			_asm
			{
				cld
				mov edi,imgptr
				mov eax,color
				mov ecx,enX
				inc ecx
				sub ecx,stX
				jc dtdlslop13
				rep stosd
dtdlslop13:
			}
#else
			{
				Int32 i;
				if ((i = enX - stX + 1) > 0)
				{
					while (i-- > 0)
						*imgptr = color;
				}
			}
#endif
		}
		else
		{
			if (y2 < 0)
				return;
			Int32 thisY;
			Int32 endY = y2;
			Int32 lastX;
			Int32 thisX;
			Int32 centX;
			thisY = y1;
			if (thisY < 0)
			{
				thisY = 0;
				thisX = MulDiv(thisY - y1, xdiff, ydiff);
			}
			else
			{
				thisX = x1;
			}
			if (endY > imgHeight)
				endY = imgHeight;

			if (xdiff >= 0)
			{
				if (thisX < 0)
				{
					thisY = y1 + MulDiv(thisX - x1, ydiff, xdiff);
					thisX = 0;
				}
				if (xdiff > 0)
				{
					lastX = y1 + MulDiv(imgWidth - x1, ydiff, xdiff);
					if (endY > lastX)
						endY = lastX;
				}
				imgptr = imgptr + (thisY * imgWidth) + thisX;
				_asm
				{
					mov edi,imgptr
					mov ebx,endY
					sub ebx,thisY
					jbe dtdlslop6
					cld

dtdlslop3:
					inc thisY
					mov eax,thisY
					sub eax,y1
					imul xdiff
					idiv ydiff
					add eax,x1
					mov edx,thisX
					mov thisX,eax
					sub eax,edx
					jz dtdlslop1
					
					mov ecx,eax
					mov eax,color
					rep stosd
					jmp dtdlslop2
dtdlslop1:
					mov eax,color
					mov dword ptr [edi],eax
dtdlslop2:
					add edi,lineW
					dec ebx
					jnz dtdlslop3
dtdlslop6:

				}
			}
			else
			{
				imgptr = imgptr + (thisY * imgWidth);
				while (thisY < endY)
				{
					lastX = thisX;
					thisX = x1 + MulDiv(thisY - y1, xdiff, ydiff);
					centX = (thisX + lastX) >> 1;
					_asm
					{
						mov edx,thisY
						cmp edx,0
						jl dtdlslop9

						mov edi,imgptr
						mov ecx,lastX
						mov ebx,centX
						mov eax,color
						cmp ebx,0
						jnl dtdlslop7
						mov ebx,0
dtdlslop7:
						cmp ecx,imgWidth
						jl dtdlslop8
						mov ecx,imgWidth
dtdlslop8:
						cmp ebx,ecx
						jg dtdlslop9
						mov dword ptr [edi+ebx * 4], eax
						inc ebx
						jmp dtdlslop8
dtdlslop9:
						mov eax,lineW
						add imgptr, eax
						inc thisY

						mov edx,thisY
						cmp edx,imgHeight
						jge dtdlslop12

						mov edi,imgptr
						mov ecx,centX
						mov ebx,thisX
						dec ecx
						mov eax,color
						cmp ebx,0
						jnl dtdlslop10
						mov ebx,0
dtdlslop10:
						cmp ecx,imgWidth
						jl dtdlslop11
						mov ecx,imgWidth
dtdlslop11:
						cmp ebx,ecx
						jg dtdlslop12
						mov dword ptr [edi+ebx * 4], eax
						inc ebx
						jmp dtdlslop11
dtdlslop12:
					}
				}
			}

		}
	}
	else
	{
		///////////////////////////////////////////////////////////
	}
}*/

void Media::DrawTool::FillRect(Media::StaticImage *img, Int32 x1, Int32  y1, Int32 x2, Int32 y2, Int32 color)
{
	if (img->info->fourcc != 0)
		return;
	if (img->info->bpp != 32)
		return;
	Int32 tlx;
	Int32 tly;
	Int32 brx;
	Int32 bry;
	if (x1 < x2)
	{
		tlx = x1;
		brx = x2;
	}
	else
	{
		tlx = x2;
		brx = x1;
	}
	if (y1 < y2)
	{
		tly = y1;
		bry = y2;
	}
	else
	{
		tly = y2;
		bry = y1;
	}
	/////////////////////////////////////
}
