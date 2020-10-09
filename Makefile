# Usage:
# make			# compiles source files
# make clean	# removes compiled files

all:
	@echo "Compiling server and client code..."
	$(CC) server.c -o server
	$(CC) client.c -o client
	@echo "Done!"

clean:
	@echo "Cleaning up..."
	rm -f server
	rm -f client
	rm -f new-file.txt