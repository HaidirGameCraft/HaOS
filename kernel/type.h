#pragma once

typedef unsigned char uint8_t;
typedef unsigned char byte;

typedef unsigned short uint16_t;
typedef unsigned short word;

typedef unsigned int uint32_t;
typedef unsigned int dword;

typedef unsigned long uint64_t;
typedef unsigned long qword;

typedef uint64_t size_t;
typedef uint64_t addr_t;

#define NULL ((void*) 0)

#define __packed __attribute__((packed))
#define __align(x) __attribute__((aligned(x)))