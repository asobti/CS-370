import time;
import sys;

'''
Just a program that spins for 30 seconds. Used to test sys_myjoin()
'''

startTime = int(time.time());

while int(time.time()) - startTime < int(sys.argv[1]):
	pass

print 'Spin process finished'
