all:
	g++ server.cpp Command.cpp TrainObject.cpp pugixml.cpp -I\include -o server -lsqlite3
	g++ client.cpp -o client -I\include -lsqlite3
clean:
	rm -f client server
