TARGET = UNISHELL

SRC = main.c read_command.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	gcc -Wall -g $^ -o $@

%.o: %.c
	gcc -Wall -g -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

# ^ means all dependencies
# < means the first dependency
