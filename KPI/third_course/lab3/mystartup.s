	;  constants
Stack_Size               EQU 0x00000400
	
	;  stack area
	AREA STACK, NOINIT, READWRITE, ALIGN = 3

Stack_Mem SPACE Stack_Size  ;  reserve stack memory
__initial_sp  ;  initial stack pointer value

	;  interrupt vector area
	AREA RESET, DATA, READONLY

	EXPORT __Vectors
__Vectors  ;  table of vectors
	DCD __initial_sp
	DCD Reset_Handler
__Vectors_End

	;  code area
	AREA |.text|, CODE, READONLY

	EXPORT Reset_Handler
Reset_Handler PROC
	B main
	ENDP

	EXTERN main_loop
main PROC  ;  main progam entry point
	
	BL main_loop
	B main  ;  endless loop
	ENDP
	ALIGN

	END