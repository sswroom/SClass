section .text

global InsertionSort_SortInt32

;void InsertionSort_SortInt32(Int32 *arr, IntOS firstIndex, IntOS lastIndex)
;0 retAddr
;rcx arr
;rdx firstIndex
;r8 lastIndex
	align 16
InsertionSort_SortInt32:
;	IntOS i;
;	IntOS j;
;	Int32 temp;
;	Int32 temp1;
;	Int32 temp2;

	push rsi
	push rdi
	mov r10d,dword [rcx+rdx*4] ;	temp1 = arr[left];
	lea rsi,[rdx + 1] ;	i = left + 1;
	cmp rsi,r8					;	while (i <= right)
	jg insertexit				;	{
	
	align 16
insertlop:
	mov r11d,dword [rcx+rsi*4]	;		temp2 = arr[i];
	cmp r10d,r11d				;		if ( temp1 > temp2 )
	jle insertlop2				;		{
	mov dword [rcx+rsi*4],r10d	;			arr[i] = temp1;
	lea rdi,[rsi-1]				;			j = i - 1;
	
	cmp rdi,rdx					;			while (j > left)
	jle insertlop4				;			{
	align 16
insertlop5:
	mov eax,dword [rcx+rdi*4-4]	;				temp = arr[j-1];
	cmp eax,r11d				;				if ( temp > temp2 )
	jle insertlop4
	mov dword [rcx+rdi*4],eax	;					arr[j--] = temp;
	dec rdi
	cmp rdi,rdx
	jg insertlop5				;				else
	align 16					;					break;
insertlop4:						;			}
	mov dword [rcx+rdi*4],r11d	;			arr[j] = temp2;
	jmp insertlop3				;		}
	align 16					;		else
insertlop2:						;		{
	mov r10,r11					;			temp1 = temp2;
	
	align 16					;		}
insertlop3:
	inc rsi						;		i++;
	cmp rsi,r8					;	}
	jle insertlop
	
	align 16
insertexit:
	pop rdi
	pop rsi
	ret
