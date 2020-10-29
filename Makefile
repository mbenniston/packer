CC=cc

all : packer

packer : packer.c
	$(CC) packer.c -o packer 

clean : 
	rm packer