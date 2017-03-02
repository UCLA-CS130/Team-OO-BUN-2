#!/bin/bash


port=2020

echo -e "port $port;\n" > test_config
echo -e "path / StaticHandler {\n   root static;\n}" >> test_config
echo -e "path /echo EchoHandler {}" >> test_config
echo -e "default NotFoundHandler {}" >> test_config
echo -e "\nconfig:"
cat test_config

./webserver test_config & PID=$!

curl -s -I http://localhost:$port/integration.html > /tmp/actual

echo ""
echo "Response: "
cat /tmp/actual

kill $PID

diff -y testdata/expected /tmp/actual


if [ $? == 0 ]; then
	echo "Expected Response Success with exit code $?"
else
	echo "Unexpected Response Fail with exit code $?"
fi

