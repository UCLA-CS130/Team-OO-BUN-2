#!/bin/bash

docker save httpserver | bzip2 | ssh -i "ubuntu.pem" ubuntu@ec2-54-241-154-77.us-west-1.compute.amazonaws.com 'bunzip2 | docker load'
ssh -t -t -i "ubuntu.pem" ubuntu@ec2-54-241-154-77.us-west-1.compute.amazonaws.com << 'EOF'
	docker stop $(docker ps -a -q)
	docker rm $(docker ps -a -q)
	docker run --rm -t -p 8080:8080 httpserver
EOF