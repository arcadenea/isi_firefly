TARGET=firefly
DEBUG = -g
CFLAGS = -w `pkg-config --cflags gtk+-2.0`
LFLAGS = `pkg-config --libs gtk+-2.0` -lm


all: $(TARGET)

OBJS += main.o result.o

rm-elf:
	-rm -f $(TARGET) $(OBJS)

$(TARGET): $(OBJS)
	$(CC)  $(OBJS) -o $(TARGET) $(LFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET) $(OBJS)
