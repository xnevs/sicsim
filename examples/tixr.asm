tixr	START	0
	LDX	#0
LOOP	TIX	DESET
	JEQ	KONEC
	LDCH	ZNAKI,X
	J	LOOP
KONEC	J	KONEC

DESET	WORD	3
ZNAKI	BYTE	1
	BYTE	2
	BYTE	3
	BYTE	4
	END	0