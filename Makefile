TARGET = build/mariodeluxe.gb
SRC    = $(wildcard src/*.c)
OBJ    = $(SRC:.c=.o)
LCC    = lcc

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(LCC) -o $@ $^

%.o: %.c
	$(LCC) -c $< -o $@

run: $(TARGET)
	emuli $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET) *.map
