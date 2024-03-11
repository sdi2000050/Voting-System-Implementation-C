mvote: mvote.o mvote_Implementation.o
	gcc mvote_Implementation.o mvote.o -o mvote -lm

mvote.o: mvote.c  
	gcc -c mvote.c -o mvote.o 

mvote_Implementation.o: mvote_Implementation.c 
	gcc -c mvote_Implementation.c -o mvote_Implementation.o 

clean:
	rm -f *.o mvote
