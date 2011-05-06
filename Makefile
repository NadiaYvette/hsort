CC:=gcc
CFLAGS:=-D_GNU_SOURCE -MMD -Iinclude -g -O2 -W -Wall
DIR:=drv heap str
include $(addsuffix /Makefile,$(DIR))
PFX:=$(shell echo $(DIR) | tr 'a-z' 'A-Z')
OBJ:=$(foreach pfx,$(PFX),$($(pfx)_DIR)/$($(pfx)_OBJ))
DRV:=$(foreach pfx,$(filter-out DRV,$(PFX)),$($(pfx)_DIR)/$($(pfx)_DRV))
DRV_OBJ:=$(addsuffix .o,$(DRV))
EXE:=hsort
LIB:=m c
ifdef DEBUG
CFLAGS:=-DDEBUG $(CFLAGS)
endif

.PHONY: clean distclean test

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $^ $(addprefix -l,$(LIB)) -o $@

-include $(addsuffix .mk,$(DRV))

-include $(OBJ:.o=.d) $(DRV_OBJ:.o=.d)

clean:
	-$(RM) $(OBJ) $(DRV_OBJ)

distclean: clean
	-$(RM) $(OBJ:.o=.d) $(DRV_OBJ:.o=.d) $(EXE) $(DRV)
