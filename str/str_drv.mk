$(STR_DIR)/$(STR_DRV): $(addprefix $(STR_DIR)/,$(STR_OBJ)) $(addsuffix .o,$(STR_DIR)/$(STR_DRV))
	$(CC) $(CFLAGS) $^ $(addprefix -l,$(LIB)) -o $@
