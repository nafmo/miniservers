all: echoserver authserver

echoserver: echoserver.cpp
	gcc -g -o echoserver echoserver.cpp

authserver: authserver.cpp
	gcc -g -o authserver authserver.cpp
