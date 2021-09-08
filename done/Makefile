## ======================================================================
## partial Makefile provided to students
##

GTK_INCLUDE := `pkg-config --cflags gtk+-3.0`
GTK_LIBS := `pkg-config --libs gtk+-3.0`

.PHONY: clean new style feedback submit1 submit2 submit

CPPLAGS += -std=c11 -Wall -pedantic -g

# a bit more checks if you'd like to uncomment
#CFLAGS += -Wextra -Wfloat-equal -Wshadow                         \
#-Wpointer-arith -Wbad-function-cast -Wcast-align -Wwrite-strings \
#-Wconversion -Wunreachable-code

# uncomment if you want to add DEBUG flag
# CPPFLAGS += -DDEBUG

# ----------------------------------------------------------------------
# feel free to update/modifiy this part as you wish

# all those libs are required on Debian, feel free to adapt it to your box
LDLIBS += -lcheck -lm -lrt -lpthread -lsubunit

# for cpu alu extern library
LDFLAGS += -L.
LDLIBS += -lcs212gbfinalext

# uncomment for blargg tests
#CPPFLAGS += -DBLARGG

# uncomment for Tetris
#CPPFLAGS += -DTETRIS_ROM_WRITE_CHECK

# for linking requiring gtk
#	$(CC) $^ $(GTK_LIBS) $(LDFLAGS) $(LDLIBS) -o $@
CFLAGS += $(GTK_INCLUDE)

final: unit-test-alu unit-test-bit unit-test-bit-vector unit-test-bus unit-test-cartridge unit-test-component unit-test-cpu unit-test-cpu-dispatch-week08 unit-test-cpu-dispatch-week09 unit-test-memory unit-test-timer test-cpu-week08 test-cpu-week09 test-gameboy gbsimulator

TARGETS := 
CHECK_TARGETS := unit-test-alu unit-test-bit unit-test-bit-vector unit-test-bus unit-test-cartridge unit-test-component unit-test-cpu unit-test-cpu-dispatch-week08 unit-test-cpu-dispatch-week09 unit-test-memory unit-test-timer unit-test-alu_ext unit-test-cpu-dispatch
OBJS = 
OBJS_NO_STATIC_TESTS =
OBJS_STATIC_TESTS = 
OBJS = $(OBJS_STATIC_TESTS) $(OBJS_NO_STATIC_TESTS)

# custom command to print command needed for external library
local:
	export LD_LIBRARY_PATH=.

# custom command to remove executables that are not unit tests
purge::
	-@/bin/rm -f test-cpu-week08 test-cpu-week09 test-gameboy gbsimulator

#-----------------------------------------------------------------------
# added

# from gcc -MM *.c
alu.o: alu.c alu.h bit.h error.h
bit.o: bit.c bit.h
bit_vector.o: bit_vector.c bit_vector.h bit.h
bootrom.o: bootrom.c bootrom.h bus.h component.h memory.h error.h bit.h \
 gameboy.h cartridge.h timer.h cpu.h alu.h opcode.h lcdc.h image.h \
 bit_vector.h joypad.h
bus.o: bus.c bus.h component.h memory.h error.h bit.h
cartridge.o: cartridge.c cartridge.h component.h memory.h error.h bus.h \
 bit.h
component.o: component.c component.h memory.h error.h
cpu-alu.o: cpu-alu.c cpu-alu.h alu.h bit.h error.h cpu.h bus.h \
 component.h memory.h opcode.h cpu-storage.h cpu-registers.h util.h \
 gameboy.h cartridge.h timer.h lcdc.h image.h bit_vector.h joypad.h
cpu.o: cpu.c cpu.h alu.h bit.h error.h bus.h component.h memory.h \
 opcode.h cpu-alu.h cpu-storage.h cpu-registers.h util.h gameboy.h \
 cartridge.h timer.h lcdc.h image.h bit_vector.h joypad.h
cpu-registers.o: cpu-registers.c cpu-registers.h cpu.h alu.h bit.h \
 error.h bus.h component.h memory.h opcode.h
cpu-storage.o: cpu-storage.c cpu-storage.h cpu.h alu.h bit.h error.h \
 bus.h component.h memory.h opcode.h cpu-registers.h util.h gameboy.h \
 cartridge.h timer.h lcdc.h image.h bit_vector.h joypad.h
error.o: error.c
gameboy.o: gameboy.c gameboy.h bus.h component.h memory.h error.h bit.h \
 cartridge.h timer.h cpu.h alu.h opcode.h lcdc.h image.h bit_vector.h \
 joypad.h bootrom.h
gbsimulator.o: gbsimulator.c sidlib.h gameboy.h bus.h component.h \
 memory.h error.h bit.h cartridge.h timer.h cpu.h alu.h opcode.h lcdc.h \
 image.h bit_vector.h joypad.h
image.o: image.c error.h image.h bit_vector.h bit.h
libsid_demo.o: libsid_demo.c sidlib.h
memory.o: memory.c memory.h error.h
opcode.o: opcode.c opcode.h bit.h
sidlib.o: sidlib.c sidlib.h
test-cpu-week08.o: test-cpu-week08.c opcode.h bit.h cpu.h alu.h error.h \
 bus.h component.h memory.h cpu-storage.h cpu-registers.h util.h \
 gameboy.h cartridge.h timer.h lcdc.h image.h bit_vector.h joypad.h
test-cpu-week09.o: test-cpu-week09.c opcode.h bit.h cpu.h alu.h error.h \
 bus.h component.h memory.h cpu-storage.h cpu-registers.h util.h \
 gameboy.h cartridge.h timer.h lcdc.h image.h bit_vector.h joypad.h
test-gameboy.o: test-gameboy.c gameboy.h bus.h component.h memory.h \
 error.h bit.h cartridge.h timer.h cpu.h alu.h opcode.h lcdc.h image.h \
 bit_vector.h joypad.h util.h
test-image.o: test-image.c error.h util.h image.h bit_vector.h bit.h \
 sidlib.h
timer.o: timer.c timer.h bit.h cpu.h alu.h error.h bus.h component.h \
 memory.h opcode.h cpu-storage.h cpu-registers.h util.h gameboy.h \
 cartridge.h lcdc.h image.h bit_vector.h joypad.h
unit-test-alu.o: unit-test-alu.c tests.h error.h alu.h bit.h
unit-test-alu_ext.o: unit-test-alu_ext.c tests.h error.h alu.h bit.h \
 alu_ext.h
unit-test-bit.o: unit-test-bit.c tests.h error.h bit.h
unit-test-bit-vector.o: unit-test-bit-vector.c tests.h error.h \
 bit_vector.h bit.h image.h
unit-test-bus.o: unit-test-bus.c tests.h error.h bus.h component.h \
 memory.h bit.h util.h
unit-test-cartridge.o: unit-test-cartridge.c tests.h error.h cartridge.h \
 component.h memory.h bus.h bit.h cpu.h alu.h opcode.h
unit-test-component.o: unit-test-component.c tests.h error.h bus.h \
 component.h memory.h bit.h
unit-test-cpu.o: unit-test-cpu.c tests.h error.h alu.h bit.h opcode.h \
 util.h cpu.h bus.h component.h memory.h cpu-registers.h cpu-storage.h \
 gameboy.h cartridge.h timer.h lcdc.h image.h bit_vector.h joypad.h \
 cpu-alu.h
unit-test-cpu-dispatch.o: unit-test-cpu-dispatch.c tests.h error.h alu.h \
 bit.h cpu.h bus.h component.h memory.h opcode.h util.h \
 unit-test-cpu-dispatch.h cpu.c cpu-alu.h cpu-storage.h cpu-registers.h \
 gameboy.h cartridge.h timer.h lcdc.h image.h bit_vector.h joypad.h
unit-test-cpu-dispatch-week08.o: unit-test-cpu-dispatch-week08.c tests.h \
 error.h alu.h bit.h cpu.h bus.h component.h memory.h opcode.h gameboy.h \
 cartridge.h timer.h lcdc.h image.h bit_vector.h joypad.h util.h \
 unit-test-cpu-dispatch.h cpu.c cpu-alu.h cpu-storage.h cpu-registers.h
unit-test-cpu-dispatch-week09.o: unit-test-cpu-dispatch-week09.c tests.h \
 error.h alu.h bit.h cpu.h bus.h component.h memory.h opcode.h util.h \
 unit-test-cpu-dispatch.h cpu.c cpu-alu.h cpu-storage.h cpu-registers.h \
 gameboy.h cartridge.h timer.h lcdc.h image.h bit_vector.h joypad.h
unit-test-memory.o: unit-test-memory.c tests.h error.h bus.h component.h \
 memory.h bit.h
unit-test-timer.o: unit-test-timer.c util.h tests.h error.h timer.h bit.h \
 cpu.h alu.h bus.h component.h memory.h opcode.h
util.o: util.c

# linking unit-tests
unit-test-alu: unit-test-alu.o error.o alu.o bit.o
unit-test-alu_ext: unit-test-alu_ext.o error.o alu.o bit.o \
 cpu-storage.o cpu-registers.o cpu-alu.o bus.o bit_vector.o image.o \
 cpu.o component.o opcode.o memory.o
unit-test-bit: unit-test-bit.o error.o bit.o
unit-test-bus: unit-test-bus.o error.o bus.o component.o \
 memory.o bit.o util.o
unit-test-cartridge: unit-test-cartridge.o error.o cartridge.o \
 component.o memory.o bus.o bit.o cpu.o alu.o opcode.o \
 cpu-registers.o cpu-alu.o cpu-storage.o bit_vector.o image.o
unit-test-component: unit-test-component.o error.o bus.o \
 component.o memory.o bit.o
unit-test-cpu: unit-test-cpu.o error.o alu.o bit.o opcode.o \
 util.o cpu.o bus.o component.o memory.o cpu-registers.o cpu-storage.o \
 gameboy.o cartridge.o timer.o image.o bit_vector.o \
 cpu-alu.o bootrom.o
unit-test-cpu-dispatch-week08: unit-test-cpu-dispatch-week08.o \
 error.o alu.o bit.o bus.o component.o memory.o opcode.o gameboy.o \
 cartridge.o timer.o image.o bit_vector.o util.o \
 cpu-alu.o cpu-storage.o cpu-registers.o bootrom.o
unit-test-cpu-dispatch-week09: unit-test-cpu-dispatch-week09.o \
 error.o alu.o bit.o bus.o component.o memory.o opcode.o util.o \
 cpu-alu.o cpu-storage.o cpu-registers.o \
 gameboy.o cartridge.o timer.o image.o bit_vector.o bootrom.o
unit-test-memory: unit-test-memory.o error.o bus.o component.o \
 memory.o bit.o
unit-test-timer: unit-test-timer.o util.o error.o timer.o bit.o \
 cpu.o alu.o bus.o component.o memory.o opcode.o cpu-storage.o \
 cpu-registers.o cpu-alu.o bit_vector.o image.o
unit-test-bit-vector: unit-test-bit-vector.o error.o \
 bit_vector.o bit.o image.o
unit-test-cpu-dispatch: unit-test-cpu-dispatch.o error.o alu.o \
 bit.o bus.o component.o memory.o opcode.o util.o \
 cpu-alu.o cpu-storage.o cpu-registers.o \
 gameboy.o cartridge.o timer.o image.o bit_vector.o bootrom.o

# linking other tests
test-cpu-week08: test-cpu-week08.o opcode.o bit.o cpu.o alu.o error.o \
 bus.o component.o memory.o cpu-storage.o cpu-registers.o util.o \
 gameboy.o cartridge.o timer.o image.o bit_vector.o bootrom.o cpu-alu.o
test-cpu-week09: test-cpu-week09.o opcode.o bit.o cpu.o alu.o error.o \
 bus.o component.o memory.o cpu-storage.o cpu-registers.o util.o \
 gameboy.o cartridge.o timer.o image.o bit_vector.o bootrom.o cpu-alu.o
test-gameboy: test-gameboy.o gameboy.o bus.o component.o memory.o \
 error.o bit.o cartridge.o timer.o cpu.o alu.o opcode.o image.o \
 bit_vector.o util.o bootrom.o cpu-storage.o cpu-registers.o \
 cpu-alu.o
test-image: test-image.o error.o util.o image.o bit_vector.o bit.o \
 sidlib.o
	$(CC) $^ $(GTK_LIBS) $(LDFLAGS) $(LDLIBS) -o $@
gbsimulator: gbsimulator.o sidlib.o gameboy.o bus.o component.o \
 memory.o error.o bit.o cartridge.o timer.o cpu.o alu.o opcode.o \
 image.o bit_vector.o cpu-storage.o cpu-registers.o cpu-alu.o \
 bootrom.o
	$(CC) $^ $(GTK_LIBS) $(LDFLAGS) $(LDLIBS) -o $@


# ----------------------------------------------------------------------
# This part is to make your life easier. See handouts how to make use of it.


clean::
	-@/bin/rm -f *.o *~ $(CHECK_TARGETS)

new: clean all

static-check:
	scan-build -analyze-headers --status-bugs -maxloop 64 make CC=clang new

style:
	astyle -n -o -A8 -xt0 *.[ch]

check:: $(CHECK_TARGETS)
	$(foreach target,$(CHECK_TARGETS),./$(target) &&) true

# target to run tests
check:: all
	@if ls tests/*.*.sh 1> /dev/null 2>&1; then \
	  for file in tests/*.*.sh; do [ -x $$file ] || echo "Launching $$file"; ./$$file || exit 1; done; \
	fi

IMAGE=chappeli/feedback:latest
feedback:
	@docker pull $(IMAGE)
	@docker run -it --rm -v ${PWD}:/home/tester/done $(IMAGE)

SUBMIT_SCRIPT=../provided/submit.sh
submit1: $(SUBMIT_SCRIPT)
	@$(SUBMIT_SCRIPT) 1

submit2: $(SUBMIT_SCRIPT)
	@$(SUBMIT_SCRIPT) 2

submit:
	@printf 'what "make submit"??\nIt'\''s either "make submit1" or "make submit2"...\n'

libsid.so: sidlib.c
	$(CC) -fPIC -shared $(CPPFLAGS) $(CFLAGS) $(GTK_INCLUDE) $^ -o $@

libsid_demo.o: CFLAGS += $(GTK_INCLUDE)

libsid_demo: LDFLAGS += -L.
libsid_demo: LDLIBS += $(GTK_LIBS) -lsid
libsid_demo: libsid_demo.o libsid.so
