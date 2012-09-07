all: echoserver authserver

echoserver: echoserver.cpp
	g++ -g -o echoserver echoserver.cpp

authserver: authserver.cpp
	g++ -g -o authserver authserver.cpp
