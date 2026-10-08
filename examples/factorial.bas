# compute 6!

	n = 6
	f = 1
loop:
	if n <= 1 then goto done
	f = f * n
	n = n - 1
	goto loop
done:
	print f
	stop
