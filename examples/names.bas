# a variable and a label can share a name

	loop = 3
loop:
	print loop
	loop = loop - 1
	if loop > 0 then goto loop
	stop
