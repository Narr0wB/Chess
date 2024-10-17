OUTD = $(OUTDIR)/movegen
TARGET = $(OUTDIR)/movegen.o

all: 
	@mkdir -p $(OUTD)
	$(CC) $(CCFLAGS) -I$(INCLUDE) -c Position.cpp -o $(OUTD)/tmp.position.o
	$(CC) $(CCFLAGS) -I$(INCLUDE) -c Tables.cpp -o $(OUTD)/tmp.tables.o
	$(CC) $(CCFLAGS) -I$(INCLUDE) -c Types.cpp -o $(OUTD)/tmp.types.o
	
	$(LD) -r -o $(TARGET) $(OUTD)/*.o
	rm $(OUTD)/tmp.*