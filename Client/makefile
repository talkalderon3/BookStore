# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
CFLAGS:=-g -Wall -Weffc++ -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -pthread

# All Targets
all: StompBookClubClient

# Tool invocations
StompBookClubClient: bin/StompBookClubClient.o bin/keyboardClient.o bin/socketClient.o bin/connectionHandler.o bin/booksDataBase.o bin/stompFrame.o
	@echo 'Building target: StompBookClubClient'
	g++ -o bin/StompBookClubClient bin/StompBookClubClient.o bin/keyboardClient.o bin/socketClient.o bin/connectionHandler.o bin/booksDataBase.o bin/stompFrame.o $(LDFLAGS)
	@echo 'Finished building target: StompBookClubClient'
	@echo ' '

# Depends on the source and header files
bin/StompBookClubClient.o: src/StompBookClubClient.cpp
	$(CC) $(CFLAGS) -c -Linclude -o bin/StompBookClubClient.o src/StompBookClubClient.cpp
bin/keyboardClient.o: src/keyboardClient.cpp
	$(CC) $(CFLAGS) -c -Linclude -o bin/keyboardClient.o src/keyboardClient.cpp
bin/socketClient.o: src/socketClient.cpp
	$(CC) $(CFLAGS) -c -Linclude -o bin/socketClient.o src/socketClient.cpp
bin/connectionHandler.o: src/connectionHandler.cpp
	$(CC) $(CFLAGS) -c -Linclude -o bin/connectionHandler.o src/connectionHandler.cpp
bin/booksDataBase.o: src/booksDataBase.cpp
	$(CC) $(CFLAGS) -c -Linclude -o bin/booksDataBase.o src/booksDataBase.cpp
bin/stompFrame.o: src/stompFrame.cpp
	$(CC) $(CFLAGS) -c -Linclude -o bin/stompFrame.o src/stompFrame.cpp

#Clean the build directory
.PHONY: clean
clean:
	rm -f bin/*