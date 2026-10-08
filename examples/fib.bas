# print the first 10 Fibonacci numbers

	a = 0
	b = 1
	n = 10
loop:
	print a
	t = a + b
	a = b
	b = t
	n = n - 1
	if n != 0 then goto loop
	stop
