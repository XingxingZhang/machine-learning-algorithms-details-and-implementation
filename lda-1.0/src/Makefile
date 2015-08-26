GXX = g++ -O3 

all : pre lda
	@echo done!

pre :
	@echo Latent Dirichlet Allocation by Xingxing Zhang

lda : main.o lda_wrapper.o lda.o
	$(GXX) -o $@ $^

main.o : main.cpp lda_wrapper.h
	$(GXX) -c main.cpp -o main.o

lda_wrapper.o : lda_wrapper.cpp lda_wrapper.h lda.h iosutil.h strid.h
	$(GXX) -c lda_wrapper.cpp -o lda_wrapper.o

lda.o : lda.h lda.cpp
	$(GXX) -c lda.cpp -o lda.o

clean:
	rm *.o
	rm lda
