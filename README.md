# ⚙️ Programming Paradigms - Low-Level Systems Exploration

*A journey through memory addresses, processor instructions, and concurrent execution - implemented in C and Assembly*

---

## 🖥️ Overview
This repository contains projects from my Programming Paradigms course, focusing on **system-level programming** and **hardware abstraction**. These implementations demonstrate my ability to work directly with memory, processor instructions, and concurrent execution flows.

**Core Technologies**  
![C](https://img.shields.io/badge/C-ANSI_C99-yellow?logo=c&logoColor=white)
![Assembly](https://img.shields.io/badge/Assembly-x86_64_(NASM)-red)
![Concurrency](https://img.shields.io/badge/Threading-POSIX_Threads-blueviolet)
![Tools](https://img.shields.io/badge/Tools-GDB%20|%20Valgrind%20|%20Make-lightgrey)

---

## 🧮 Projects

### [Custom Memory Allocator](/memory-manager)
- Implemented `malloc`, `free`, and `realloc` from scratch
- **Techniques**: Pointer arithmetic, memory boundary tags
- **Optimization**: 92% heap utilization with coalescing strategy
- **Assembly Integration**: SSE instructions for alignment checks

### [Multithreaded Web Server](/concurrent-server)
- POSIX Threads (pthreads) implementation
- **Features**: Thread pooling, mutex-protected request queue
- **Benchmark**: Handled 1500+ concurrent connections
- **Low-Level I/O**: Direct socket syscalls without libc wrappers

### [Assembly Image Processor](/asm-optimizations)
- x86-64 NASM routines for bitmap manipulation
- **Optimizations**: SIMD (AVX2) parallel pixel processing
- **C Integration**: Inline assembly for performance-critical sections
- **Result**: 18x speedup over naive C implementation

---

## 🛠️ Technical Arsenal

### **Memory Management**
```c
void* heap_start = sbrk(0);  // Direct brk manipulation
struct block_header* blk = (struct block_header*)((char*)ptr - sizeof(struct block_header));
