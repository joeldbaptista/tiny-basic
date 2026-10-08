# print the primes below 200 by trial division

	n = 2
next:
	if n >= 200 then goto done
	d = 2
trial:
	if d * d > n then goto prime
	# r is the remainder of n / d
	r = n - n / d * d
	if r == 0 then goto skip
	d = d + 1
	goto trial
prime:
	print n
skip:
	n = n + 1
	goto next
done:
	stop
