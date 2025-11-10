CC = gcc
# CFLAGS defines compilation options:
# -Wall: Enable all standard warnings
# -Wextra: Enable extra warnings
# -std=c11: Use the C11 standard
CFLAGS = -Wall -Wextra -std=c11

# LDFLAGS defines linking options:
# -mconsole: Necessary for MinGW/Windows to use main() as entry point (fixes WinMain error)
# -pthread: Links the POSIX threads library
LDFLAGS = -mconsole -pthread

TARGET = bank_c
SOURCE = main.c

# 'all' is the default target, which builds the application
all: $(TARGET)

# Rule to compile the main executable
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)

# 'run' target for quick compilation and execution
run: $(TARGET)
	./$(TARGET)

# 'clean' target to remove compiled files
clean:
	rm -f $(TARGET)