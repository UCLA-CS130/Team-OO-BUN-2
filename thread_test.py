import socket
import sys, os
import subprocess
import time
import signal

def multithread_test():
	try:
		server = subprocess.Popen("exec ./webserver thread_config", shell=True)

		time.sleep(2)

		host = 'localhost'
		port = 2020
		buffer_size = 1024

		partial = 'GET /echo '
		rest = 'HTTP/1.1\r\n\r\n'

		complete = 'GET /status HTTP/1.1\r\n\r\n'

		s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s1.settimeout(5)

		s1.connect((host, port))
		s1.send(partial)
		print("Sent Echo Handler Partial Request")

		s2.connect((host, port))
		s2.send(complete)
		print("Sent Status Handler Complete Request")

		d2 = s2.recv(buffer_size)
		s2.close()
		print("Completed Status Handler Request")

		s1.send(rest)
		d1 = s1.recv(buffer_size)
		s1.close()
		print("Completed Echo Handler Request")

		server.kill()
		return True

	except socket.timeout:
		server.kill()
		return False
		
	except KeyboardInterrupt:
		server.kill()




file = open("thread_config", 'w')

file.write("port 2020;\n")
file.write("path /static StaticHandler {\n   root static;\n}\n")
file.write("path /echo EchoHandler {}\n")
file.write("path /status StatusHandler {}\n")
file.write("default NotFoundHandler {}\n")

file.close()


result = multithread_test()
if result:
	print "\nMultithread Test Passed"
else:
	print "\nMultithread Test Failed"
