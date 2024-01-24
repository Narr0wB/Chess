CC = g++
INCLUDE = -I./include
LIBPATH =
LIB =
COPTIONS =-g -O3
TARGET = build/Engine.dll

OBJS = build/EntryPoint.o build/Engine.o build/Log.o build/Transposition.o build/Evaluate.o build/types.o build/tables.o build/position.o


$(TARGET): $(OBJS)
	$(CC) -shared $(LIBPATH) $(COPTIONS) $(OBJS) -o $(TARGET) $(LIB)

build/EntryPoint.o: Engine/src/EntryPoint.cpp Engine/src/Engine.h Engine/src/Evaluate.h
	$(CC) $(INCLUDE) $(COPTIONS) -c $< -o $@
 
build/Engine.o: Engine/src/Engine.cpp Engine/src/Engine.h Engine/src/Evaluate.h
	$(CC) $(INCLUDE) $(COPTIONS) -c $< -o $@

build/Log.o: Engine/src/Log.cpp Engine/src/Log.h
	$(CC) $(INCLUDE) $(COPTIONS) -c $< -o $@

build/Transposition.o: Engine/src/Transposition.cpp Engine/src/Transposition.h
	$(CC) $(INCLUDE) $(COPTIONS) -c $< -o $@

build/Evaluate.o: Engine/src/Evaluate.cpp Engine/src/Evaluate.h
	$(CC) $(INCLUDE) $(COPTIONS) -c $< -o $@

build/types.o: Engine/src/types.cpp Engine/src/types.h
	$(CC) $(INCLUDE) $(COPTIONS) -c $< -o $@

build/tables.o: Engine/src/tables.cpp Engine/src/tables.h
	$(CC) $(INCLUDE) $(COPTIONS) -c $< -o $@

build/position.o: Engine/src/position.cpp Engine/src/position.h
	$(CC) $(INCLUDE) $(COPTIONS) -c $< -o $@

clean:
	rm build/*.o
	powershell rm build/*.exe