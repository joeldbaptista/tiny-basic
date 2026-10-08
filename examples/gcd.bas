# gcd(1071, 462) by repeated subtraction

	a = 1071
	b = 462
loop:
	if a == b then goto done
	if a > b then goto suba
	b = b - a
	goto loop
suba:
	a = a - b
	goto loop
done:
	print a
	stop
