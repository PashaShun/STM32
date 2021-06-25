; Initialize Stack Size
; Equ 400 hex (1024) bytes
Stack_Size      EQU     0x00000400

; Area STACK (NOINIT and READ/WRITE)
                AREA    STACK, NOINIT, READWRITE, ALIGN=3
; Initialize memory equals Stack_Size
Stack_Mem       SPACE   Stack_Size
__initial_sp

; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  Reset_Handler             [WEAK]
__Vectors
                DCD     __initial_sp             ; Top of Stack
                DCD     Reset_Handler            ; Reset Handler
__Vectors_End

                AREA    DATA , DATA, READWRITE, ALIGN=3
ARIPHMETIC		DCD 0
LOGIC			DCD 0

X				EQU 0x20
Y				EQU 0x05
Z				EQU	0x04

A				EQU 0x50
B				EQU 0x01
C				EQU 0x33
D				EQU 0xE2	
				
				AREA    CODE , CODE, READONLY, ALIGN=3
 
Reset_Handler   PROC

                LDR     R0, =__main
                BX      R0
                ENDP

__main      	PROC

				mov r0, #X
				mov r1, #Y
				mov	r2,	#Z
				
				mov r5, #A
				mov r6, #B
				mov r7, #C
				mov r8, #D
				
				; (X + Y) + (Y - Z) - (X - Y) = Q     ;X = 20h,	Y = 05h,	Z = 04h;
	
				add r3, r0, r1 ;r3 = (X + Y)
				sub r4, r1, r2 ;r4 = (Y - Z)
				add r2, r3, r4 ;r2 = (X + Y) + (Y - Z)
				sub r4, r0, r1 ;r4 = (X - Y)
				sub r3, r2, r4 ;r3 = (X + Y) + (Y - Z) - (X - Y)
				
				; Q = !A and B and !C or (!B and C and D xor A and C), A = 50h, B = 01, C = 33h, D = E2h
				
				mvn r9, r6 ;r9 = !A
				and r10, r9, r7 ;r10 = r9 and r7 = !B and C
				and r10, r10, r8 ; r10 = r10 and r7 = !B and C and D
				and r9, r5, r7 ; r9 = r5 and r7 = A and C
				eor r9, r10, r9 ; r9 = r10 xor r9 = (!B and C and D xor A and C)
				and r9, r8, r9 ;r9 = r8 and r9 = D and !C
				mvn r4, r5 ; r4 = !A
				and r10, r4, r6; r9 = !A and B
				mvn r8, r7 ; r8 = !C
				and r10, r10, r8; r9 = !A and B and !C
				orr r10, r10, r9 ;r10 = !A and B and !C or (!B and C and D xor A and C)
				
				LDR r0, =ARIPHMETIC
				STR r3,  [r0]
				LDR r0, =LOGIC
				STR r10, [r0]
__mainloop	

		B __mainloop
		ENDP			
		END
