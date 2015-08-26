
CC=g++ -O3
OBJDIR=obj

all : pre svm-train svm-predict
	@echo done!

pre :
	@if [ ! -d $(OBJDIR) ]; then	mkdir $(OBJDIR); fi

svm-train : svm-train.cpp $(OBJDIR)/kernelmatrix.o $(OBJDIR)/svmsolver.o $(OBJDIR)/svm.o
	$(CC) $^ -o svm-train

svm-predict : svm-predict.cpp $(OBJDIR)/kernelmatrix.o $(OBJDIR)/svmsolver.o $(OBJDIR)/svm.o
	$(CC) $^ -o svm-predict

$(OBJDIR)/kernelmatrix.o : KernelMatrix.cpp KernelMatrix.h Cache.h SVM_Type.h
	$(CC) -c KernelMatrix.cpp -o $@

$(OBJDIR)/svmsolver.o : SVM_Solver.cpp SVM_Solver.h KernelMatrix.h
	$(CC) -c SVM_Solver.cpp -o $@

$(OBJDIR)/svm.o : SVM.cpp SVM.h SVM_Solver.h
	$(CC) -c SVM.cpp -o $@

clean:
	rm svm-train
	rm svm-predict
	rm -r $(OBJDIR)
