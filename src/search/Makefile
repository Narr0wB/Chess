OUTD = $(OUTDIR)/search
TARGET = $(OUTDIR)/search.o

all: 
	@mkdir -p $(OUTD)
	$(CC) $(CCFLAGS) -I$(INCLUDE) -c Search.cpp -o $(OUTD)/tmp.search.o
	$(CC) $(CCFLAGS) -I$(INCLUDE) -c Thread.cpp -o $(OUTD)/tmp.thread.o
	$(CC) $(CCFLAGS) -I$(INCLUDE) -c Transposition.cpp -o $(OUTD)/tmp.transposition.o
	$(CC) $(CCFLAGS) -I$(INCLUDE) -c Evaluate.cpp -o $(OUTD)/tmp.eval.o
	
	$(LD) -r -o $(TARGET) $(OUTD)/*.o
	rm $(OUTD)/tmp.*