.global strcmp_sve
strcmp_sve:
	ptrue p5.b
	setffr
	
	mov x5, #0

l_loop:
	ldff1b z0.b, p5/z, [x0, x5]
	ldff1b z1.b, p5/z, [x1, x5]
	rdffrs p7.b, p5/z
	b.nlast l_fault

	incb x5
	cmpeq p0.b, p5/z, z0.b, #0
	cmpne p1.b, p5/z, z0.b, z1.b
l_test:
	orrs p4.b, p5/z, p0.b, p1.b
	b.none l_loop

l_retrun:
	brkb p4.b, p5/z, p4.b
	lasta w0, p4, z0.b
	lasta w1, p4, z1.b
	sub w0, w0, w1
	ret

l_fault:
	incp x5, p7.b
	setffr
	cmpeq p0.b, p7/z, z0.b, #0
	cmpne p1.b, p7/z, z0.b, z1.b
	b l_test

