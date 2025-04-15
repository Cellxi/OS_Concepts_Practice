CFLAGS = -Wall
TARGET1 = producer
TARGET2 = consumer

all: $(TARGET1) $(TARGET2)

$(TARGET1):$(TARGET1).c
	gcc $(CFLAGS) $(TARGET1).c -o $(TARGET1)

$(TARGET2):$(TARGET2).c
	gcc $(CFLAGS) $(TARGET2).c -o $(TARGET2)

clean:
	rm -f program1 program2
