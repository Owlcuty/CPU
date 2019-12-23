CC = gcc
CSAN = -Wall -Wextra
CO = -o
MAIN = cpu.cpp
OFILES = cpu.opp
LINKS =
LFLAGS = -lm
all:
	$(CC) $(CSAN) $(CO) $(OFILES) $(MAIN) $(LINKS) $(LFLAGS)
