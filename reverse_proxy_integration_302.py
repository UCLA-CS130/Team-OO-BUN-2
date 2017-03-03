from os import system
from threading import Thread
import socket
import time
import sys
import subprocess

def setup_socket():
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	return s;

def client_thread(error_code, REQUEST, E_RESPONSE):
	TCP_IP = '127.0.0.1'
	TCP_PORT = 2020
	BUFFER_SIZE = 1024
	MESSAGE = REQUEST

	s = setup_socket()
	s.connect(('localhost', TCP_PORT))
	s.send(MESSAGE)
	data = s.recv(BUFFER_SIZE)
	s.close()

	if (E_RESPONSE not in data):
	    	print "Failure response: ", repr(data)
                error_code[0] = 1
		return

	error_code[0] = 0

if __name__ == "__main__":
	print "\n-----Opening server-----"
	p = subprocess.Popen("./webserver ./testdata/test_config_three", shell=True)

	time.sleep(4)

	error_code = [0]
	REQUEST = "GET /proxy/ HTTP/1.0\r\n\r\n"
	EXPECTED_RESPONSE = "200 OK"
	thread_client_four = Thread(target = client_thread, args=(error_code, REQUEST, EXPECTED_RESPONSE))
	thread_client_four.start()
	thread_client_four.join()

	print "\n\n-----Closing the server/Doing cleanup-----"
	system("fuser -k 2020/tcp")

	time.sleep(2)


	if (error_code[0] == 1):
		print "\n\nTEST FAILURE\n"
		exit(1)
	else:
		print "\n\nTEST SUCCESS\n"
		exit(0)
