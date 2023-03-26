#include <coreinit/memorymap.h>
#include <whb/log.h>
#include <whb/log_udp.h>

#include <coreinit/cache.h>
#include <wums.h>

WUMS_MODULE_EXPORT_NAME("homebrew_kernel");
WUMS_MODULE_SKIP_INIT_FINI();
WUMS_MODULE_INIT_BEFORE_RELOCATION_DONE_HOOK();

#define KERN_SYSCALL_TBL1 0xFFE84C70 //Unknown
#define KERN_SYSCALL_TBL2 0xFFE85070 //Games
#define KERN_SYSCALL_TBL3 0xFFE85470 //Loader
#define KERN_SYSCALL_TBL4 0xFFEAAA60 //Home menu
#define KERN_SYSCALL_TBL5 0xFFEAAE60 //Browser

typedef struct sr_table_t {
    uint32_t value[16];
    uint32_t sdr1;
} sr_table_t;

extern "C" void SCKernelCopyData(uint32_t dst, uint32_t src, uint32_t len);

extern "C" void KernelCopyDataInternal(uint32_t dst, uint32_t src, uint32_t len);

extern "C" void KernelWriteSRsInternal(sr_table_t *table);

extern "C" void KernelReadSRsInternal(sr_table_t *table);

void kernelInitialize();

void KernelReadSRsInternalFunc(sr_table_t *table) {
    uint32_t i = 0;

    // calculate PT_size ((end-start)*8/4096)*4 or (end-start)/128
    // Minimum page table size is 64Kbytes.

    asm volatile("eieio; isync");

    asm volatile("mfspr %0, 25"
                 : "=r"(table->sdr1));

    asm volatile("mfsr %0, 0"
                 : "=r"(table->value[i]));
    i++;
    asm volatile("mfsr %0, 1"
                 : "=r"(table->value[i]));
    i++;
    asm volatile("mfsr %0, 2"
                 : "=r"(table->value[i]));
    i++;
    asm volatile("mfsr %0, 3"
                 : "=r"(table->value[i]));
    i++;
    asm volatile("mfsr %0, 4"
                 : "=r"(table->value[i]));
    i++;
    asm volatile("mfsr %0, 5"
                 : "=r"(table->value[i]));
    i++;
    asm volatile("mfsr %0, 6"
                 : "=r"(table->value[i]));
    i++;
    asm volatile("mfsr %0, 7"
                 : "=r"(table->value[i]));
    i++;
    asm volatile("mfsr %0, 8"
                 : "=r"(table->value[i]));
    i++;
    asm volatile("mfsr %0, 9"
                 : "=r"(table->value[i]));
    i++;
    asm volatile("mfsr %0, 10"
                 : "=r"(table->value[i]));
    i++;
    asm volatile("mfsr %0, 11"
                 : "=r"(table->value[i]));
    i++;
    asm volatile("mfsr %0, 12"
                 : "=r"(table->value[i]));
    i++;
    asm volatile("mfsr %0, 13"
                 : "=r"(table->value[i]));
    i++;
    asm volatile("mfsr %0, 14"
                 : "=r"(table->value[i]));
    i++;
    asm volatile("mfsr %0, 15"
                 : "=r"(table->value[i]));
    i++;

    asm volatile("eieio; isync");
}

void KernelWriteSRsInternalFunc(sr_table_t *table) {
    asm volatile("eieio; isync");

    // Writing didn't work for all at once so we only write number 8.
    // TODO: fix this and change it if required.

    /*asm volatile("mtsr 0, %0" : : "r" (table->value[i])); i++;
    asm volatile("mtsr 1, %0" : : "r" (table->value[i])); i++;
    asm volatile("mtsr 2, %0" : : "r" (table->value[i])); i++;
    asm volatile("mtsr 3, %0" : : "r" (table->value[i])); i++;
    asm volatile("mtsr 4, %0" : : "r" (table->value[i])); i++;
    asm volatile("mtsr 5, %0" : : "r" (table->value[i])); i++;*/
    asm volatile("mtsr 6, %0"
                 :
                 : "r"(table->value[6]));
    asm volatile("mtsr 7, %0"
                 :
                 : "r"(table->value[7]));
    asm volatile("mtsr 8, %0"
                 :
                 : "r"(table->value[8]));
    /*asm volatile("mtsr 9, %0" : : "r" (table->value[i])); i++;
    asm volatile("mtsr 10, %0" : : "r" (table->value[i])); i++;
    asm volatile("mtsr 11, %0" : : "r" (table->value[i])); i++;
    asm volatile("mtsr 12, %0" : : "r" (table->value[i])); i++;
    asm volatile("mtsr 13, %0" : : "r" (table->value[i])); i++;
    asm volatile("mtsr 14, %0" : : "r" (table->value[i])); i++;
    asm volatile("mtsr 15, %0" : : "r" (table->value[i])); i++;*/


    asm volatile("isync");
}

void KernelCopyData(uint32_t dst, uint32_t src, uint32_t len) {
    KernelCopyDataInternal(dst, src, len);
}

void KernelWriteSRs(sr_table_t *table) {
    KernelWriteSRsInternal(table);
}

void KernelReadSRs(sr_table_t *table) {
    KernelReadSRsInternal(table);
}


/* Write a 32-bit word with kernel permissions */
void __attribute__((noinline)) kern_write(void *addr, uint32_t value) {
    asm volatile(
            "stwu 1, -0x10(1)\n"
            "stw 2,  0x08(1)\n"
            "stw 13, 0x0C(1)\n"
            "li 3,1\n"
            "li 4,0\n"
            "mr 5,%1\n"
            "li 6,0\n"
            "li 7,0\n"
            "lis 8,1\n"
            "mr 9,%0\n"
            "mr %1,1\n"
            "li 0,0x3500\n"
            "sc\n"
            "nop\n"
            "mr 1,%1\n"
            "lwz 2, 0x8(1)\n"
            "lwz 13, 0xC(1)\n"
            "addi 1, 1, 0x10\n"
            :
            : "r"(addr), "r"(value)
            : "memory", "ctr", "lr", "0", "3", "4", "5", "6", "7", "8", "9", "10",
              "11", "12");
}

/* Read a 32-bit word with kernel permissions */
uint32_t __attribute__((noinline)) kern_read(const void *addr) {
    uint32_t result;
    asm volatile(
            "li 3,1\n"
            "li 4,0\n"
            "li 5,0\n"
            "li 6,0\n"
            "li 7,0\n"
            "lis 8,1\n"
            "mr 9,%1\n"
            "li 0,0x3400\n"
            "mr %0,1\n"
            "sc\n"
            "nop\n"
            "mr 1,%0\n"
            "mr %0,3\n"
            : "=r"(result)
            : "b"(addr)
            : "memory", "ctr", "lr", "0", "3", "4", "5", "6", "7", "8", "9", "10",
              "11", "12");

    return result;
}

void KernelPatchSyscall(int index, uint32_t addr) {
    kern_write((void *) (KERN_SYSCALL_TBL1 + index * 4), addr);
    kern_write((void *) (KERN_SYSCALL_TBL2 + index * 4), addr);
    kern_write((void *) (KERN_SYSCALL_TBL3 + index * 4), addr);
    kern_write((void *) (KERN_SYSCALL_TBL4 + index * 4), addr);
    kern_write((void *) (KERN_SYSCALL_TBL5 + index * 4), addr);
}

void kernelInitialize() {
    static uint8_t ucSyscallsSetupRequired = 1;
    if (!ucSyscallsSetupRequired) {
        return;
    }
    ucSyscallsSetupRequired = 0;

    KernelPatchSyscall(0x25, (uint32_t) SCKernelCopyData);
    KernelPatchSyscall(0x36, (uint32_t) KernelReadSRsInternalFunc);
    KernelPatchSyscall(0x0A, (uint32_t) KernelWriteSRsInternalFunc);
}

WUMS_INITIALIZE(myargs) {
    kernelInitialize();
}

WUMS_EXPORT_FUNCTION(KernelCopyData);
WUMS_EXPORT_FUNCTION(KernelWriteSRs);
WUMS_EXPORT_FUNCTION(KernelReadSRs);
WUMS_EXPORT_FUNCTION(KernelPatchSyscall);