; FILE test.as
; Should be assembled without any errors
NUMS1:	.data	1,2
NUMS2:	.data	1, 11, 111, -1, -11, -111
NUMS3:	.data	1, 2, 3, 4, 5, 6, -1, -2, -3, -4, -5, -6
STRING:	.string "hello guys, how are you feeling today?"
STRUCT1: .struct		123, "yooooooooooooooooooo"   
STRUCT2: .struct		-123, "this is a struct, very cool"   

	.extern		A1
	.extern		BOOGA34543
	.entry		LABEL
	.entry		HEY

macro m1
	HEY: .struct -5, "boo" 
	LABEL: .data 128
endmacro

MAIN:	mov r1, r5
		mov STRUCT1.1, A1
		m1
		cmp STRUCT2.2, #505
		cmp r5, r5
		sub BOOGA34543.1, HEY
		not LABEL
		clr r7
		lea A1, A1
		macro MmMmMm
			hlt
		endmacro
		inc STRUCT1.1
		dec r2
		jmp MAIN
tomato:	bne NUMS3
		get NUMS2
		prn #69
		jsr tomato
		rts
		MmMmMm
