
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef uint32_t size_t;
typedef uint32_t addr_t;

#define true 1
#define false 0
#define bool unsigned char
#define NULL 0

#define VOID void
#define BYTE uint8_t
#define WORD uint16_t
#define DWORD uint32_t
#define QWORD uint64_t

#define C_PACKED __attribute__((packed))

extern void HALT();