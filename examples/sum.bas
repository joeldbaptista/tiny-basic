# sum the integers from 1 to 100

	i = 1
	sum = 0
loop:
	if i > 100 then goto done
	sum = sum + i
	i = i + 1
	goto loop
done:
	print sum
	stop
