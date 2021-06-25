		AREA    FUNCTION, CODE, READONLY
		EXPORT signed_sum
				
signed_sum 		PROC
		MOV		r3, #0
loop 
		LDRB	r2, [r0]
		ADDS	r0, #4   
		ADDS	r3, r2
		CMP 	r2, #0
		BNE 	loop
		
		STRB	r3, [r1]
		
		BX 		lr
		
		ENDP
			
		END