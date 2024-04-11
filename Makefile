# ITS BROKEN
GPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-leading-underscore -fno-exceptions
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = loader.o kernal.o

# Create a .o file from the kernal
%.o: %.cpp
	g++ $(GPPPARAMS) -o $@ -c $<

# Create a .o file from the loader
%.o: %.s
	as $(ASPARAMS) -o $@ $<

# Use the kernal and loader to make the bin
mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)
# Copy the bin to the boot directory
install: mykernel.bin
	sudo cp $< /boot/mykernel.bin
