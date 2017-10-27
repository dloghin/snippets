#!/usr/bin/python
#
# Python code to get the permutations in the range [x0,x1] using backtracking
# Dumi Loghin 2016
#

import sys, time

def fact(n):
	f = 1;
	for i in range(2,n+1):
		f *= i
	return f

def cont(x, k, x0, x1):
	f = []
	for i in range(0, x1-x0+1):
		f.append(False)
	for i in range(0, k+1):
		if f[x[i]-x0]:
			return False
		f[x[i]-x0] = True
	return True

def proc(x):
	print x

# Iterative backtracking
def bk_iterative(x0, x1):
	x = []
	for i in range(x0, x1+1):
		x.append(i)

	n = x1 - x0 + 1
	nsol = 0
	k = 0
	x[0] = x0-1
	while k >= 0:
		if x[k] < x1:
			x[k] += 1
		else:
			k -= 1
			continue
		if not cont(x, k, x0, x1):
			continue
		if k == n-1:
			proc(x)
			nsol += 1
		else:
			k += 1
			x[k] = x0-1

	print "No. of permutations: " + str(nsol) + " (expected " + str(fact(n)) + ")"

# Recursive backtracking
def bk_rec(x, k, n, x0, x1, nsol):
	while x[k] < x1:
		x[k] += 1

		if cont(x, k, x0, x1):
			if k == n-1:
				proc(x)
				nsol[0] += 1
			else:
				x[k+1] = x0-1
				bk_rec(x, k+1, n, x0, x1, nsol)

# Recursive backtracking (wrapper function)
def bk_recursive(x0, x1):
	x = []
	for i in range(x0, x1+1):
		x.append(i)

	n = x1 - x0 + 1
	nsol = [0]
	x[0] = x0-1

	bk_rec(x, 0, n, x0, x1, nsol)

	print "No. of permutations: " + str(nsol[0]) + " (expected " + str(fact(n)) + ")"

def main(args):
	if len(args) < 3:
		print "Usage: " + args[0] + " " + "<start> <stop>"
		return

	x0 = int(args[1])
	x1 = int(args[2])
	if x1 <= x0:
		print "Invalid range [" + args[1] + "," + args[2] + "]"
		return
		
	print "Generating permutations of range [" + args[1] + "," + args[2] + "]"
	print "1. Using recursive backtracking..."
	time0 = time.time()
	bk_recursive(x0, x1)
	time1 = time.time()
	print "...took " + str(time1-time0) + " seconds"
	print "2. Using iterative backtracking..."
	time0 = time.time()
	bk_iterative(x0, x1)
	time1 = time.time()
	print "...took " + str(time1-time0) + " seconds"

if __name__ == "__main__":
	main(sys.argv)
