void initializeDisplay() {
    // Video memory starts at address 0xb8000 for text mode
    volatile unsigned short* VideoMemory = (volatile unsigned short*)0xb8000;
    const int screenWidth = 80;
    const int screenHeight = 25;
    for (int y = 0; y < screenHeight; y++) {
        for (int x = 0; x < screenWidth; x++) {
            VideoMemory[y * screenWidth + x] = (0x0F << 8) | ' '; 
        }
    }
}
// print shit i guess
void printf(char* str) {
    unsigned short* VideoMemory = (unsigned short*)0xb8000;
    static int col = 0;
    static int row = 0;

    for(int i = 0; str[i] != '\0'; ++i) {
        switch(str[i]) {
            case '\n':
                row++;
                col = 0;
                break;
            default:
                VideoMemory[row * 80 + col] = (VideoMemory[row * 80 + col] & 0xFF00) | str[i];
                col++;
                break;
        }
        if(col >= 80) {
            col = 0;
            row++;
        }
        if(row >= 25) {
            for(int y = 0; y < 25; y++) {
                for(int x = 0; x < 80; x++) {
                    VideoMemory[y * 80 + x] = (VideoMemory[(y + 1) * 80 + x] & 0xFF00) | ' ';
                }
            }
            row = 24;
        }
    }
}

// reads byte from io port
unsigned char inb(unsigned short port) {
    unsigned char data;
    asm volatile("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

#define LCG_MULTIPLIER 1103515245
#define LCG_INCREMENT 12345
#define LCG_MODULUS 0x80  // 2^31
unsigned int lcg_state = 0;

void lcg_seed(unsigned int seed) {
    lcg_state = seed;
}

// Function to generate a pseudo-random number using the LCG
unsigned int lcg_rand() {
    lcg_state = (LCG_MULTIPLIER * lcg_state + LCG_INCREMENT) % LCG_MODULUS;
    return lcg_state;
}
// writes bye to io port
void outb(unsigned short port, unsigned char data) {
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}
int strcmp(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return str1[i] - str2[i]; // diferanc in asci
        }
        i++;
    }
    if (str1[i] == '\0' && str2[i] == '\0') {
        return 0; // Strings are equal
    } else if (str1[i] == '\0') {
        return -1; // str1 is shorter
    } else {
        return 1; // str2 is shorter
    }
}


// Extern C so G++ doesn't edit the name of kernel main
extern "C" void kernalMain(void* multiboot_structure, unsigned int magicnum) {
    printf("hai\n");
    initializeDisplay();
    char command[256]; 
    int commandIndex = 0;

    while(1) {
        char input = inb(0x60);  
        if(input == '\0') continue;  
        
        if(input == '\b') {
            if(commandIndex > 0) {
                commandIndex--;
                outb(0x3F8, '\b');
                outb(0x3F8, ' ');
                outb(0x3F8, '\b');
            }
        } else if(input == '\n') {
            // Execute the command
            command[commandIndex] = '\0';
            if(strcmp(command, "shutdown") == 0) {
                outb(0x3F8, '\n');
                printf("dying...\n");
                asm volatile("cli");
                asm volatile("hlt");
            }
            if(strcmp(command, "killram") == 0) {
                volatile short* memory = (volatile short*)0xb8000; // Start of physical RAM
                for (unsigned long long i = 0; i < 0x800; ++i) {
                    *memory = lcg_rand() % 256;
                    memory++;
                }
            } else {
                printf("\n yea uh we didnt make interpretor code\n");
            }
            commandIndex = 0;
            printf("\n> ");
        } else {
            command[commandIndex++] = input;
            outb(0x3F8, input);
        }
    }
}
