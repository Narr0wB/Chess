OUTD = $(OUTDIR)/uci
TARGET = $(OUTDIR)/uci.o

all: 
	@mkdir -p $(OUTD)
	$(CC) $(CCFLAGS) -I$(INCLUDE) -c UCI.cpp -o $(OUTD)/tmp.uci.o
	
	$(LD) -r -o $(TARGET) $(OUTD)/*.o
	rm $(OUTD)/tmp.*