section .text

global RGBColorFilter_ProcessImagePart
global RGBColorFilter_ProcessImageHDRPart
global RGBColorFilter_ProcessImageHDRDPart
global RGBColorFilter_ProcessImageHDRDLPart

;void RGBColorFilter_ProcessImagePart(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, UInt8 *lut, Int32 bpp)

;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx srcPtr r10
;rdx destPtr r11
;r8 width
;r9 height
;72 sbpl
;80 dbpl
;88 lut
;96 bpp

	align 16
RGBColorFilter_ProcessImagePart:
	mov r10,rcx
	mov r11,rdx
	mov eax,dword [rsp+64] ;bpp
	cmp eax,32
	jz RGBColorFilter_ProcessImagePart32
	cmp eax,48
	jz RGBColorFilter_ProcessImagePart48
	ret
	
	align 16
RGBColorFilter_ProcessImagePart32:
	push rbp
	push rbx
	push rsi
	push rdi
	mov qword [rsp+64],r9
	mov rbx,qword [rsp+88] ;lut
	pxor xmm4,xmm4
	align 16
pip32_0:
	mov rcx,r8 ;width
	mov rsi,r10 ;srcPtr
	mov rdi,r11 ;destPtr
	align 16
pip32_1:
	movzx rax,byte [rsi]
	movsx rdx,word [rbx+rax*2]
	pinsrw xmm4,rdx,0
	movd xmm0,edx
	movdqa xmm2,xmm0
	movdqa xmm3,xmm0
	movzx eax,byte [rsi+1]
	movsx edx,word [rbx+rax*2]
	pinsrw xmm4,rdx,1
	movd xmm0,rdx
	pminsw xmm2,xmm0
	pmaxsw xmm3,xmm0
	movzx rax,byte [rsi+2]
	movsx rdx,word [rbx+rax*2]
	pinsrw xmm4,rdx,2
	mov al,byte [rsi+3]
	movd xmm0,rdx
	pminsw xmm2,xmm0
	pmaxsw xmm3,xmm0
	movd rdx,xmm2 ;minV
	mov [rdi+3],al
	cmp edx,8192
	jge pip32_5
	movd rax,xmm3 ;maxV
	cmp rax,8192
	jle pip32_4

	mov r9,rax
	mov rbp,rdx
	pextrw rax,xmm4,0
	movsx rax,ax
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	sub r9,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi],dl

	pextrw rax,xmm4,1
	movsx rax,ax
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+1],dl

	pextrw rax,xmm4,2
	movsx rax,ax
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+2],dl

	jmp pip32_6

	align 16
pip32_5:
	mov al,byte [rbx+8704]
	mov byte [rdi],al
	mov byte [rdi+1],al
	mov byte [rdi+2],al
	jmp pip32_6

	align 16
pip32_4:
	pextrw rax,xmm4,0
	mov dl,byte [rbx+rax+512]
	mov byte [rdi],dl
	pextrw rax,xmm4,1
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+1],dl
	pextrw rax,xmm4,2
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+2],dl

	align 16
pip32_6:
	add rsi,4
	add rdi,4
	dec rcx
	jnz pip32_1

	add r10,qword [rsp+72] ;sbpl srcPtr
	add r11,qword [rsp+80] ;dbpl destPtr
	dec qword [rsp+64] ;height
	jnz pip32_0
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

	align 16
RGBColorFilter_ProcessImagePart48:
	push rbp
	push rbx
	push rsi
	push rdi
	mov qword [rsp+64],r9
	mov rbx,qword [rsp+88] ;lut
	pxor xmm4,xmm4
	align 16
pip48_0:
	mov rcx,r8 ;width
	mov rsi,r10 ;srcPtr
	mov rdi,r11 ;destPtr
	align 16
pip48_1:
	movzx rax,word [rsi]
	movsx rdx,word [rbx+rax*2]
	pinsrw xmm4,rdx,0
	movd xmm0,rdx
	movdqa xmm2,xmm0
	movdqa xmm3,xmm0
	movzx rax,word [rsi+2]
	movsx rdx,word [rbx+rax*2]
	pinsrw xmm4,rdx,1
	movd xmm0,rdx
	pminsw xmm2,xmm0
	pmaxsw xmm3,xmm0
	movzx rax,word [rsi+4]
	movsx rdx,word [rbx+rax*2]
	pinsrw xmm4,rdx,2
	movd xmm0,rdx
	pminsw xmm2,xmm0
	pmaxsw xmm3,xmm0
	movd rdx,xmm2 ;minV
	cmp rdx,8192
	jge pip48_5
	movd rax,xmm3 ;maxV
	cmp rax,8192
	jle pip48_4

	mov r9,rax
	mov rbp,rdx
	pextrw rax,xmm4,0
	movsx rax,ax
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	sub r9,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi],dx

	pextrw rax,xmm4,1
	movsx rax,ax
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+2],dx

	pextrw rax,xmm4,2
	movsx rax,ax
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+4],dx

	jmp pip48_6

	align 16
pip48_5:
	mov ax,word [rbx+147456]
	mov word [rdi],ax
	mov word [rdi+2],ax
	mov word [rdi+4],ax
	jmp pip48_6

	align 16
pip48_4:
	pextrw rax,xmm4,0
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi],dx
	pextrw rax,xmm4,1
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+2],dx
	pextrw rax,xmm4,2
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+4],dx

	align 16
pip48_6:
	add rsi,6
	add rdi,6
	dec rcx
	jnz pip48_1

	add r10,qword [rsp+72] ;sbpl srcPtr
	add r11,qword [rsp+80] ;dbpl destPtr
	dec qword [rsp+64] ;height
	jnz pip48_0
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void RGBColorFilter_ProcessImageHDRPart(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, UInt8 *lut, Int32 bpp)

;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx srcPtr r10
;rdx destPtr r11
;r8 width
;r9 height
;72 sbpl
;80 dbpl
;88 lut
;96 bpp

	align 16
RGBColorFilter_ProcessImageHDRPart:
	mov r10,rcx
	mov r11,rdx
	mov eax,dword [rsp+64] ;bpp
	cmp eax,32
	jz RGBColorFilter_ProcessImageHDRPart32
	cmp eax,48
	jz RGBColorFilter_ProcessImageHDRPart48
	ret
	
	align 16
RGBColorFilter_ProcessImageHDRPart32:
	push rbp
	push rbx
	push rsi
	push rdi
	mov qword [rsp+64],r9
	mov rbx,qword [rsp+88] ;lut
	pxor xmm4,xmm4
	align 16
pihp32_0:
	mov rcx,r8 ;width
	mov rsi,r10 ;srcPtr
	mov rdi,r11 ;destPtr
	align 16
pihp32_1:
	movzx rax,byte [rsi]
	movsx rdx,word [rbx+rax*2]
	pinsrw xmm4,rdx,0
	movd xmm0,edx
	movdqa xmm2,xmm0
	movdqa xmm3,xmm0
	movzx eax,byte [rsi+1]
	movsx edx,word [rbx+rax*2]
	pinsrw xmm4,rdx,1
	movd xmm0,rdx
	pminsw xmm2,xmm0
	pmaxsw xmm3,xmm0
	movzx rax,byte [rsi+2]
	movsx rdx,word [rbx+rax*2]
	pinsrw xmm4,rdx,2
	mov al,byte [rsi+3]
	movd xmm0,rdx
	pminsw xmm2,xmm0
	pmaxsw xmm3,xmm0
;	movd rdx,xmm2 ;minV
	mov [rdi+3],al
;	cmp edx,8192
;	jge pihp32_5
	xor rdx,rdx
	movd rax,xmm3 ;maxV
	movsx rax,ax
	cmp rax,8192
	jle pihp32_4

	mov r9,rax
	mov rbp,rdx
	pextrw rax,xmm4,0
	movsx rax,ax
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	sub r9,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi],dl

	pextrw rax,xmm4,1
	movsx rax,ax
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+1],dl

	pextrw rax,xmm4,2
	movsx rax,ax
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+2],dl

	jmp pihp32_6

	align 16
pihp32_5:
	mov al,byte [rbx+8704]
	mov byte [rdi],al
	mov byte [rdi+1],al
	mov byte [rdi+2],al
	jmp pihp32_6

	align 16
pihp32_4:
	pextrw rax,xmm4,0
	mov dl,byte [rbx+rax+512]
	mov byte [rdi],dl
	pextrw rax,xmm4,1
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+1],dl
	pextrw rax,xmm4,2
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+2],dl

	align 16
pihp32_6:
	add rsi,4
	add rdi,4
	dec rcx
	jnz pihp32_1

	add r10,qword [rsp+72] ;sbpl srcPtr
	add r11,qword [rsp+80] ;dbpl destPtr
	dec qword [rsp+64] ;height
	jnz pihp32_0
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

	align 16
RGBColorFilter_ProcessImageHDRPart48:
	push rbp
	push rbx
	push rsi
	push rdi
	mov qword [rsp+64],r9
	mov rbx,qword [rsp+88] ;lut
	pxor xmm4,xmm4
	align 16
pihp48_0:
	mov rcx,r8 ;width
	mov rsi,r10 ;srcPtr
	mov rdi,r11 ;destPtr
	align 16
pihp48_1:
	movzx rax,word [rsi]
	movsx rdx,word [rbx+rax*2]
	pinsrw xmm4,rdx,0
	movd xmm0,rdx
	movdqa xmm2,xmm0
	movdqa xmm3,xmm0
	movzx rax,word [rsi+2]
	movsx rdx,word [rbx+rax*2]
	pinsrw xmm4,rdx,1
	movd xmm0,rdx
	pminsw xmm2,xmm0
	pmaxsw xmm3,xmm0
	movzx rax,word [rsi+4]
	movsx rdx,word [rbx+rax*2]
	pinsrw xmm4,rdx,2
	movd xmm0,rdx
	pminsw xmm2,xmm0
	pmaxsw xmm3,xmm0
;	movd rdx,xmm2 ;minV
;	cmp rdx,8192
;	jge pihp48_5
	xor rdx,rdx
	movd rax,xmm3 ;maxV
	movsx rax,ax
	cmp rax,8192
	jle pihp48_4

	mov r9,rax
	mov rbp,rdx
	pextrw rax,xmm4,0
	movsx rax,ax
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	sub r9,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi],dx

	pextrw rax,xmm4,1
	movsx rax,ax
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+2],dx

	pextrw rax,xmm4,2
	movsx rax,ax
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+4],dx

	jmp pihp48_6

	align 16
pihp48_5:
	mov ax,word [rbx+147456]
	mov word [rdi],ax
	mov word [rdi+2],ax
	mov word [rdi+4],ax
	jmp pihp48_6

	align 16
pihp48_4:
	pextrw rax,xmm4,0
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi],dx
	pextrw rax,xmm4,1
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+2],dx
	pextrw rax,xmm4,2
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+4],dx

	align 16
pihp48_6:
	add rsi,6
	add rdi,6
	dec rcx
	jnz pihp48_1

	add r10,qword [rsp+72] ;sbpl srcPtr
	add r11,qword [rsp+80] ;dbpl destPtr
	dec qword [rsp+64] ;height
	jnz pihp48_0
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;void RGBColorFilter_ProcessImageHDRDPart(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, UInt8 *lut, Int32 bpp)

;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx srcPtr r10
;rdx destPtr r11
;r8 width
;r9 height
;72 sbpl
;80 dbpl
;88 lut
;96 bpp

	align 16
RGBColorFilter_ProcessImageHDRDPart:
	mov r10,rcx
	mov r11,rdx
	mov eax,dword [rsp+64] ;bpp
	cmp eax,32
	jz RGBColorFilter_ProcessImageHDRDPart32
	cmp eax,48
	jz RGBColorFilter_ProcessImageHDRDPart48
	ret
	
	align 16
RGBColorFilter_ProcessImageHDRDPart32:
	push rbp
	push rbx
	push rsi
	push rdi
	mov qword [rsp+64],r9
	mov rbx,qword [rsp+88] ;lut
	pxor xmm4,xmm4
	align 16
pihdp32_0:
	mov rcx,r8 ;width
	mov rsi,r10 ;srcPtr
	mov rdi,r11 ;destPtr
	align 16
pihdp32_1:
	movzx rax,byte [rsi]
	mov edx,dword [rbx+rax*4+66048]
	pinsrd xmm4,edx,0
	movd xmm0,edx
	movdqa xmm2,xmm0
	movdqa xmm3,xmm0
	movzx eax,byte [rsi+1]
	mov edx,dword [rbx+rax*4+66048]
	pinsrd xmm4,edx,1
	movd xmm0,rdx
	pminsd xmm2,xmm0
	pmaxsd xmm3,xmm0
	movzx rax,byte [rsi+2]
	mov edx,dword [rbx+rax*4+66048]
	pinsrd xmm4,edx,2
	mov al,byte [rsi+3]
	movd xmm0,rdx
	pminsd xmm2,xmm0
	pmaxsd xmm3,xmm0
;	movd rdx,xmm2 ;minV
	mov [rdi+3],al
;	cmp edx,8192
;	jge pihdp32_5
	xor rdx,rdx
	movd rax,xmm3 ;maxV
	cdqe
	cmp rax,8192
	jle pihdp32_4

	mov r9,rax
	mov rbp,rdx
	pextrd eax,xmm4,0
	cdqe
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	sub r9,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi],dl

	pextrd eax,xmm4,1
	cdqe
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+1],dl

	pextrd eax,xmm4,2
	cdqe
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+2],dl

	jmp pihdp32_6

	align 16
pihdp32_5:
	mov al,byte [rbx+8704]
	mov byte [rdi],al
	mov byte [rdi+1],al
	mov byte [rdi+2],al
	jmp pihdp32_6

	align 16
pihdp32_4:
	xor rax,rax
	pextrd eax,xmm4,0
	and eax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi],dl
	pextrd eax,xmm4,1
	and eax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+1],dl
	pextrd eax,xmm4,2
	and eax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+2],dl

	align 16
pihdp32_6:
	add rsi,4
	add rdi,4
	dec rcx
	jnz pihdp32_1

	add r10,qword [rsp+72] ;sbpl srcPtr
	add r11,qword [rsp+80] ;dbpl destPtr
	dec qword [rsp+64] ;height
	jnz pihdp32_0
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

	align 16
RGBColorFilter_ProcessImageHDRDPart48:
	push rbp
	push rbx
	push rsi
	push rdi
	mov qword [rsp+64],r9
	mov rbx,qword [rsp+88] ;lut
	pxor xmm4,xmm4
	align 16
pihdp48_0:
	mov rcx,r8 ;width
	mov rsi,r10 ;srcPtr
	mov rdi,r11 ;destPtr
	align 16
pihdp48_1:
	movzx rax,word [rsi]
	mov edx,dword [rbx+rax*4+262144]
	pinsrd xmm4,edx,0
	movd xmm0,rdx
	movdqa xmm2,xmm0
	movdqa xmm3,xmm0
	movzx rax,word [rsi+2]
	mov edx,dword [rbx+rax*4+262144]
	pinsrd xmm4,edx,1
	movd xmm0,rdx
	pminsd xmm2,xmm0
	pmaxsd xmm3,xmm0
	movzx rax,word [rsi+4]
	mov edx,dword [rbx+rax*4+262144]
	pinsrd xmm4,edx,2
	movd xmm0,rdx
	pminsd xmm2,xmm0
	pmaxsd xmm3,xmm0
;	movd rdx,xmm2 ;minV
;	cmp rdx,8192
;	jge pihdp48_5
	xor rdx,rdx
	movd rax,xmm3 ;maxV
	cdqe
	cmp rax,8192
	jle pihdp48_4

	mov r9,rax
	mov rbp,rdx
	pextrd eax,xmm4,0
	cdqe
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	sub r9,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi],dx

	pextrd eax,xmm4,1
	cdqe
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+2],dx

	pextrd eax,xmm4,2
	cdqe
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+4],dx

	jmp pihdp48_6

	align 16
pihdp48_5:
	mov ax,word [rbx+147456]
	mov word [rdi],ax
	mov word [rdi+2],ax
	mov word [rdi+4],ax
	jmp pihdp48_6

	align 16
pihdp48_4:
	xor rax,rax
	pextrd eax,xmm4,0
	and eax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi],dx
	pextrd eax,xmm4,1
	and eax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+2],dx
	pextrd eax,xmm4,2
	and eax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+4],dx

	align 16
pihdp48_6:
	add rsi,6
	add rdi,6
	dec rcx
	jnz pihdp48_1

	add r10,qword [rsp+72] ;sbpl srcPtr
	add r11,qword [rsp+80] ;dbpl destPtr
	dec qword [rsp+64] ;height
	jnz pihdp48_0
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret


;void RGBColorFilter_ProcessImageHDRDLPart(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, UInt8 *lut, Int32 bpp, Int32 hdrLev)

;0 r12
;8 rdi
;16 rsi
;24 rbx
;32 rbp
;40 retAddr
;rcx srcPtr r10
;rdx destPtr r11
;r8 width
;r9 height
;80 sbpl
;88 dbpl
;96 lut
;104 bpp
;112 hdrLev

	align 16
RGBColorFilter_ProcessImageHDRDLPart:
	mov r10,rcx
	mov r11,rdx
	mov eax,dword [rsp+64] ;bpp
	cmp eax,32
	jz RGBColorFilter_ProcessImageHDRDLPart32
	cmp eax,48
	jz RGBColorFilter_ProcessImageHDRDLPart48
	ret
	
	align 16
RGBColorFilter_ProcessImageHDRDLPart32:
	push rbp
	push rbx
	push rsi
	push rdi
	push r12
	mov qword [rsp+72],r9
	mov rbx,qword [rsp+96] ;lut
	mov ecx,dword [rsp+112] ;hdrLev
	pxor xmm4,xmm4
	align 16
pihdlp32_0:
	mov r12,r8 ;width
	mov rsi,r10 ;srcPtr
	mov rdi,r11 ;destPtr
	align 16
pihdlp32_1:
	movzx rax,byte [rsi]
	mov edx,dword [rbx+rax*4+66048]
	pinsrd xmm4,edx,0
	movd xmm0,edx
	movdqa xmm2,xmm0
	movdqa xmm3,xmm0
	movzx eax,byte [rsi+1]
	mov edx,dword [rbx+rax*4+66048]
	pinsrd xmm4,edx,1
	movd xmm0,rdx
	pminsd xmm2,xmm0
	pmaxsd xmm3,xmm0
	movzx rax,byte [rsi+2]
	mov edx,dword [rbx+rax*4+66048]
	pinsrd xmm4,edx,2
	mov al,byte [rsi+3]
	movd xmm0,rdx
	pminsd xmm2,xmm0
	pmaxsd xmm3,xmm0
	movd rdx,xmm2 ;minV
	mov [rdi+3],al
	sar edx,cl
	cmp edx,8192
	jge pihdlp32_5
	movsx rdx,dx
	movd rax,xmm3 ;maxV
	cdqe
	cmp rax,8192
	jle pihdlp32_4

	mov r9,rax
	mov rbp,rdx
	pextrd eax,xmm4,0
	cdqe
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	sub r9,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi],dl

	pextrd eax,xmm4,1
	cdqe
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+1],dl

	pextrd eax,xmm4,2
	cdqe
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+2],dl

	jmp pihdlp32_6

	align 16
pihdlp32_5:
	mov al,byte [rbx+8704]
	mov byte [rdi],al
	mov byte [rdi+1],al
	mov byte [rdi+2],al
	jmp pihdlp32_6

	align 16
pihdlp32_4:
	xor rax,rax
	pextrd eax,xmm4,0
	and eax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi],dl
	pextrd eax,xmm4,1
	and eax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+1],dl
	pextrd eax,xmm4,2
	and eax,0xffff
	mov dl,byte [rbx+rax+512]
	mov byte [rdi+2],dl

	align 16
pihdlp32_6:
	add rsi,4
	add rdi,4
	dec r12
	jnz pihdlp32_1

	add r10,qword [rsp+80] ;sbpl srcPtr
	add r11,qword [rsp+88] ;dbpl destPtr
	dec qword [rsp+72] ;height
	jnz pihdlp32_0
	pop r12
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

	align 16
RGBColorFilter_ProcessImageHDRDLPart48:
	push rbp
	push rbx
	push rsi
	push rdi
	push r12
	mov qword [rsp+72],r9
	mov rbx,qword [rsp+96] ;lut
	mov ecx,dword [rsp+112] ;hdrLev
	pxor xmm4,xmm4
	align 16
pihdlp48_0:
	mov r12,r8 ;width
	mov rsi,r10 ;srcPtr
	mov rdi,r11 ;destPtr
	align 16
pihdlp48_1:
	movzx rax,word [rsi]
	mov edx,dword [rbx+rax*4+262144]
	pinsrd xmm4,edx,0
	movd xmm0,rdx
	movdqa xmm2,xmm0
	movdqa xmm3,xmm0
	movzx rax,word [rsi+2]
	mov edx,dword [rbx+rax*4+262144]
	pinsrd xmm4,edx,1
	movd xmm0,rdx
	pminsd xmm2,xmm0
	pmaxsd xmm3,xmm0
	movzx rax,word [rsi+4]
	mov edx,dword [rbx+rax*4+262144]
	pinsrd xmm4,edx,2
	movd xmm0,rdx
	pminsd xmm2,xmm0
	pmaxsd xmm3,xmm0
	movd rdx,xmm2 ;minV
	sar edx,cl
	cmp edx,8192
	jge pihdlp48_5
	movsx rdx,dx
	movd rax,xmm3 ;maxV
	cdqe
	cmp rax,8192
	jle pihdlp48_4

	mov r9,rax
	mov rbp,rdx
	pextrd eax,xmm4,0
	cdqe
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	sub r9,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi],dx

	pextrd eax,xmm4,1
	cdqe
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+2],dx

	pextrd eax,xmm4,2
	cdqe
	mov rdx,8192
	sub rax,rbp
	sub rdx,rbp
	imul rdx
	idiv r9
	add rax,rbp
	and rax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+4],dx

	jmp pihdlp48_6

	align 16
pihdlp48_5:
	mov ax,word [rbx+147456]
	mov word [rdi],ax
	mov word [rdi+2],ax
	mov word [rdi+4],ax
	jmp pihdlp48_6

	align 16
pihdlp48_4:
	xor rax,rax
	pextrd eax,xmm4,0
	and eax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi],dx
	pextrd eax,xmm4,1
	and eax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+2],dx
	pextrd eax,xmm4,2
	and eax,0xffff
	mov dx,word [rbx+rax*2+131072]
	mov word [rdi+4],dx

	align 16
pihdlp48_6:
	add rsi,6
	add rdi,6
	dec r12
	jnz pihdlp48_1

	add r10,qword [rsp+80] ;sbpl srcPtr
	add r11,qword [rsp+88] ;dbpl destPtr
	dec qword [rsp+72] ;height
	jnz pihdlp48_0
	pop r12
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
