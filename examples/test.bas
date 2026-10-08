# the test program from the book, with labels

	a = 10
	b = 20
	c = 30
loop:
	if a * b >= 12 * c then goto done
	print -a-5
	a = a + 1
	goto loop
done:
	print a * a
	stop
