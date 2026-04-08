CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET_EXEC = kama-e
TARGET_COMP = kama-c

all: ${TARGET_EXEC} ${TARGET_COMP}

$(TARGET_EXEC): Kama/kama_execute.c
		$(CC) $(CFLAGS) -o $(TARGET_EXEC) Kama/kama_execute.c

$(TARGET_COMP): Kama/kama_compile.c
		$(CC) $(CFLAGS) -o $(TARGET_COMP) Kama/kama_compile.c

run: all
		./$(TARGET_COMP)
		./$(TARGET_EXEC) examples/study.gb

clean:
		rm -f $(TARGET_EXEC) $(TARGET_COMP) examples/*.gb