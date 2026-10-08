10 n = 2
20 if n >= 200 then 130
30 d = 2
40 if d * d > n then 100
50 r = n - n / d * d
60 if r == 0 then 110
70 d = d + 1
80 goto 40
100 print n
110 n = n + 1
120 goto 20
130 stop
