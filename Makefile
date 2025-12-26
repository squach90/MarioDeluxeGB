# --------------------
# Paths
# --------------------
SRC_DIR   = src
ASSET_DIR = assets
INC_DIR   = includes
BUILD_DIR = build

# --------------------
# Target
# --------------------
TARGET = $(BUILD_DIR)/mariodeluxe.gb

# --------------------
# Sources & objects
# --------------------
SRC  = $(wildcard $(SRC_DIR)/*.c)
OBJ  = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

# --------------------
# Tools & flags
# --------------------
LCC    = lcc
CFLAGS = -I$(INC_DIR) -I$(ASSET_DIR)

.PHONY: all run clean dirs

# --------------------
# Build
# --------------------
all: dirs $(TARGET)

dirs:
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJ)
	$(LCC) -o $@ $^

# Compile .c -> build/.o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(LCC) $(CFLAGS) -c $< -o $@

# --------------------
# Run
# --------------------
run: all
	java -jar ~/Emulicious/Emulicious.jar $(TARGET)

# --------------------
# Clean
# --------------------
clean:
	rm -rf $(BUILD_DIR)
