$(HEAP_DIR)/$(HEAP_DRV): $(addprefix $(HEAP_DIR)/,$(HEAP_OBJ)) $(addprefix $(STR_DIR)/,$(STR_OBJ))		\
			$(addsuffix .o,$(HEAP_DIR)/$(HEAP_DRV))
	$(CC) $(CFLAGS) $^ $(addprefix -l,$(LIB)) -o $@
