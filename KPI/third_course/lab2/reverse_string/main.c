
__asm void reverse(const char *src, char *dstr)
{
	MOV		r3, r0
	SUBS	r3, #1
check_end_loop 
	LDRB	r2, [r0]
	ADDS	r0, #1 
	CMP 	r2, #0
	BNE 	check_end_loop
	SUBS	r0, #2
	
reverse_loop
loop
	LDRB	r2, [r0]
	SUBS	r0, #1
	CMP		r2, #108
	BE		loop
	STRB	r2, [r1]
	ADDS	r1, #1
	CMP		r0, r3
	BNE		reverse_loop
	BX 		lr
}

int main(void)
{
	const char a[] = "Hello world!";
	char b[20];

		reverse(a, b);
	
		while (1);
}
