.set MAGIC, 0x1badb002
.set FLAGS, (1<<0 | 1<<1)
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .text
.extern kernelMain
.global loader

# Load the kernal and get into an inifinte loop
loader:
    mov $kernel_stack, %esp
    push %eax
    push %ebx
    call kernelMain
# incase for whatever reason the loader escapes the loop, trap it in another one.
_stop:
    cli
    hlt
    jmp _stop


.section .bss
.space 2*1024*1024 # 2 mb
kernel_stack: