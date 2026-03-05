CC = g++

CFLAGS = -Wall `root-config --cflags`

CLIBS = `root-config --glibs` `root-config --cflags`

all: PhotonCountAna

PhotonCountAna: PhotonCountAna.C
	$(CC) PhotonCountAna.C -o PhotonCountAna $(CLIBS) -std=c++17

clean:
	rm -rf *o