vib	START	0
LOOP	TD	AA
	JEQ	LOOP
	RD	AA
	WD	ENA
	J	LOOP

ENA	BYTE	1
AA	BYTE	0xAA