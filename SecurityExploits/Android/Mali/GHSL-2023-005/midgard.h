#ifndef MIDGARD_H
#define MIDGARD_H

//Generated using pandecode-standalone: https://gitlab.freedesktop.org/panfrost/pandecode-standalone

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <inttypes.h>
#include <string.h>

#define pan_section_ptr(base, A, S) \
        ((void *)((uint8_t *)(base) + MALI_ ## A ## _SECTION_ ## S ## _OFFSET))

#define pan_section_pack(dst, A, S, name)                                                         \
   for (MALI_ ## A ## _SECTION_ ## S ## _TYPE name = { MALI_ ## A ## _SECTION_ ## S ## _header }, \
        *_loop_terminate = (void *) (dst);                                                        \
        __builtin_expect(_loop_terminate != NULL, 1);                                             \
        ({ MALI_ ## A ## _SECTION_ ## S ## _pack(pan_section_ptr(dst, A, S), &name);              \
           _loop_terminate = NULL; }))


static inline uint64_t
__gen_uint(uint64_t v, uint32_t start, uint32_t end)
{
#ifndef NDEBUG
   const int width = end - start + 1;
   if (width < 64) {
      const uint64_t max = (1ull << width) - 1;
      assert(v <= max);
   }
#endif

   return v << start;
}

static inline uint64_t
__gen_unpack_uint(const uint8_t *restrict cl, uint32_t start, uint32_t end)
{
   uint64_t val = 0;
   const int width = end - start + 1;
   const uint64_t mask = (width == 64 ? ~0 : (1ull << width) - 1 );

   for (int byte = start / 8; byte <= end / 8; byte++) {
      val |= ((uint64_t) cl[byte]) << ((byte - start / 8) * 8);
   }

   return (val >> (start % 8)) & mask;
}

enum mali_job_type {
        MALI_JOB_TYPE_NOT_STARTED            =      0,
        MALI_JOB_TYPE_NULL                   =      1,
        MALI_JOB_TYPE_WRITE_VALUE            =      2,
        MALI_JOB_TYPE_CACHE_FLUSH            =      3,
        MALI_JOB_TYPE_COMPUTE                =      4,
        MALI_JOB_TYPE_VERTEX                 =      5,
        MALI_JOB_TYPE_GEOMETRY               =      6,
        MALI_JOB_TYPE_TILER                  =      7,
        MALI_JOB_TYPE_FUSED                  =      8,
        MALI_JOB_TYPE_FRAGMENT               =      9,
};

enum mali_write_value_type {
        MALI_WRITE_VALUE_TYPE_CYCLE_COUNTER  =      1,
        MALI_WRITE_VALUE_TYPE_SYSTEM_TIMESTAMP =      2,
        MALI_WRITE_VALUE_TYPE_ZERO           =      3,
        MALI_WRITE_VALUE_TYPE_IMMEDIATE_8    =      4,
        MALI_WRITE_VALUE_TYPE_IMMEDIATE_16   =      5,
        MALI_WRITE_VALUE_TYPE_IMMEDIATE_32   =      6,
        MALI_WRITE_VALUE_TYPE_IMMEDIATE_64   =      7,
};


struct MALI_WRITE_VALUE_JOB_PAYLOAD {
   uint64_t                             address;
   enum mali_write_value_type           type;
   uint64_t                             immediate_value;
};

struct MALI_JOB_HEADER {
   uint32_t                             exception_status;
   uint32_t                             first_incomplete_task;
   uint64_t                             fault_pointer;
   bool                                 is_64b;
   enum mali_job_type                   type;
   bool                                 barrier;
   bool                                 invalidate_cache;
   bool                                 suppress_prefetch;
   bool                                 enable_texture_mapper;
   bool                                 relax_dependency_1;
   bool                                 relax_dependency_2;
   uint32_t                             index;
   uint32_t                             dependency_1;
   uint32_t                             dependency_2;
   uint64_t                             next;
};


static inline void
MALI_JOB_HEADER_pack(uint32_t * restrict cl,
                     const struct MALI_JOB_HEADER * restrict values)
{
   cl[ 0] = __gen_uint(values->exception_status, 0, 31);
   cl[ 1] = __gen_uint(values->first_incomplete_task, 0, 31);
   cl[ 2] = __gen_uint(values->fault_pointer, 0, 63);
   cl[ 3] = __gen_uint(values->fault_pointer, 0, 63) >> 32;
   cl[ 4] = __gen_uint(values->is_64b, 0, 0) |
            __gen_uint(values->type, 1, 7) |
            __gen_uint(values->barrier, 8, 8) |
            __gen_uint(values->invalidate_cache, 9, 9) |
            __gen_uint(values->suppress_prefetch, 11, 11) |
            __gen_uint(values->enable_texture_mapper, 12, 12) |
            __gen_uint(values->relax_dependency_1, 14, 14) |
            __gen_uint(values->relax_dependency_2, 15, 15) |
            __gen_uint(values->index, 16, 31);
   cl[ 5] = __gen_uint(values->dependency_1, 0, 15) |
            __gen_uint(values->dependency_2, 16, 31);
   cl[ 6] = __gen_uint(values->next, 0, 63);
   cl[ 7] = __gen_uint(values->next, 0, 63) >> 32;
}


#define MALI_JOB_HEADER_LENGTH 32
struct mali_job_header_packed { uint32_t opaque[8]; };
static inline void
MALI_JOB_HEADER_unpack(const uint8_t * restrict cl,
                       struct MALI_JOB_HEADER * restrict values)
{
   if (((const uint32_t *) cl)[4] & 0x2400) fprintf(stderr, "XXX: Invalid field unpacked at word 4\n");
   values->exception_status = __gen_unpack_uint(cl, 0, 31);
   values->first_incomplete_task = __gen_unpack_uint(cl, 32, 63);
   values->fault_pointer = __gen_unpack_uint(cl, 64, 127);
   values->is_64b = __gen_unpack_uint(cl, 128, 128);
   values->type = __gen_unpack_uint(cl, 129, 135);
   values->barrier = __gen_unpack_uint(cl, 136, 136);
   values->invalidate_cache = __gen_unpack_uint(cl, 137, 137);
   values->suppress_prefetch = __gen_unpack_uint(cl, 139, 139);
   values->enable_texture_mapper = __gen_unpack_uint(cl, 140, 140);
   values->relax_dependency_1 = __gen_unpack_uint(cl, 142, 142);
   values->relax_dependency_2 = __gen_unpack_uint(cl, 143, 143);
   values->index = __gen_unpack_uint(cl, 144, 159);
   values->dependency_1 = __gen_unpack_uint(cl, 160, 175);
   values->dependency_2 = __gen_unpack_uint(cl, 176, 191);
   values->next = __gen_unpack_uint(cl, 192, 255);
}

static inline const char *
mali_job_type_as_str(enum mali_job_type imm)
{
    switch (imm) {
    case MALI_JOB_TYPE_NOT_STARTED: return "Not started";
    case MALI_JOB_TYPE_NULL: return "Null";
    case MALI_JOB_TYPE_WRITE_VALUE: return "Write value";
    case MALI_JOB_TYPE_CACHE_FLUSH: return "Cache flush";
    case MALI_JOB_TYPE_COMPUTE: return "Compute";
    case MALI_JOB_TYPE_VERTEX: return "Vertex";
    case MALI_JOB_TYPE_GEOMETRY: return "Geometry";
    case MALI_JOB_TYPE_TILER: return "Tiler";
    case MALI_JOB_TYPE_FUSED: return "Fused";
    case MALI_JOB_TYPE_FRAGMENT: return "Fragment";
    default: return "XXX: INVALID";
    }
}

static inline void
MALI_JOB_HEADER_print(FILE *fp, const struct MALI_JOB_HEADER * values, unsigned indent)
{
   fprintf(fp, "%*sException Status: %u\n", indent, "", values->exception_status);
   fprintf(fp, "%*sFirst Incomplete Task: %u\n", indent, "", values->first_incomplete_task);
   fprintf(fp, "%*sFault Pointer: 0x%" PRIx64 "\n", indent, "", values->fault_pointer);
   fprintf(fp, "%*sIs 64b: %s\n", indent, "", values->is_64b ? "true" : "false");
   fprintf(fp, "%*sType: %s\n", indent, "", mali_job_type_as_str(values->type));
   fprintf(fp, "%*sBarrier: %s\n", indent, "", values->barrier ? "true" : "false");
   fprintf(fp, "%*sInvalidate Cache: %s\n", indent, "", values->invalidate_cache ? "true" : "false");
   fprintf(fp, "%*sSuppress Prefetch: %s\n", indent, "", values->suppress_prefetch ? "true" : "false");
   fprintf(fp, "%*sEnable Texture Mapper: %s\n", indent, "", values->enable_texture_mapper ? "true" : "false");
   fprintf(fp, "%*sRelax Dependency 1: %s\n", indent, "", values->relax_dependency_1 ? "true" : "false");
   fprintf(fp, "%*sRelax Dependency 2: %s\n", indent, "", values->relax_dependency_2 ? "true" : "false");
   fprintf(fp, "%*sIndex: %u\n", indent, "", values->index);
   fprintf(fp, "%*sDependency 1: %u\n", indent, "", values->dependency_1);
   fprintf(fp, "%*sDependency 2: %u\n", indent, "", values->dependency_2);
   fprintf(fp, "%*sNext: 0x%" PRIx64 "\n", indent, "", values->next);
}

static inline void
MALI_WRITE_VALUE_JOB_PAYLOAD_pack(uint32_t * restrict cl,
                                  const struct MALI_WRITE_VALUE_JOB_PAYLOAD * restrict values)
{
   cl[ 0] = __gen_uint(values->address, 0, 63);
   cl[ 1] = __gen_uint(values->address, 0, 63) >> 32;
   cl[ 2] = __gen_uint(values->type, 0, 31);
   cl[ 3] = 0;
   cl[ 4] = __gen_uint(values->immediate_value, 0, 63);
   cl[ 5] = __gen_uint(values->immediate_value, 0, 63) >> 32;
}


#define MALI_WRITE_VALUE_JOB_PAYLOAD_LENGTH 24
#define MALI_WRITE_VALUE_JOB_PAYLOAD_header 0


struct mali_write_value_job_payload_packed { uint32_t opaque[6]; };
static inline void
MALI_WRITE_VALUE_JOB_PAYLOAD_unpack(const uint8_t * restrict cl,
                                    struct MALI_WRITE_VALUE_JOB_PAYLOAD * restrict values)
{
   if (((const uint32_t *) cl)[3] & 0xffffffff) fprintf(stderr, "XXX: Invalid field unpacked at word 3\n");
   values->address = __gen_unpack_uint(cl, 0, 63);
   values->type = __gen_unpack_uint(cl, 64, 95);
   values->immediate_value = __gen_unpack_uint(cl, 128, 191);
}

static inline const char *
mali_write_value_type_as_str(enum mali_write_value_type imm)
{
    switch (imm) {
    case MALI_WRITE_VALUE_TYPE_CYCLE_COUNTER: return "Cycle Counter";
    case MALI_WRITE_VALUE_TYPE_SYSTEM_TIMESTAMP: return "System Timestamp";
    case MALI_WRITE_VALUE_TYPE_ZERO: return "Zero";
    case MALI_WRITE_VALUE_TYPE_IMMEDIATE_8: return "Immediate 8";
    case MALI_WRITE_VALUE_TYPE_IMMEDIATE_16: return "Immediate 16";
    case MALI_WRITE_VALUE_TYPE_IMMEDIATE_32: return "Immediate 32";
    case MALI_WRITE_VALUE_TYPE_IMMEDIATE_64: return "Immediate 64";
    default: return "XXX: INVALID";
    }
}

static inline void
MALI_WRITE_VALUE_JOB_PAYLOAD_print(FILE *fp, const struct MALI_WRITE_VALUE_JOB_PAYLOAD * values, unsigned indent)
{
   fprintf(fp, "%*sAddress: 0x%" PRIx64 "\n", indent, "", values->address);
   fprintf(fp, "%*sType: %s\n", indent, "", mali_write_value_type_as_str(values->type));
   fprintf(fp, "%*sImmediate Value: 0x%" PRIx64 "\n", indent, "", values->immediate_value);
}

struct mali_write_value_job_packed {
   uint32_t opaque[14];
};

#define MALI_JOB_HEADER_header                  \
   .is_64b = true

#define MALI_WRITE_VALUE_JOB_LENGTH 56
#define MALI_WRITE_VALUE_JOB_SECTION_HEADER_TYPE struct MALI_JOB_HEADER
#define MALI_WRITE_VALUE_JOB_SECTION_HEADER_header MALI_JOB_HEADER_header
#define MALI_WRITE_VALUE_JOB_SECTION_HEADER_pack MALI_JOB_HEADER_pack
#define MALI_WRITE_VALUE_JOB_SECTION_HEADER_unpack MALI_JOB_HEADER_unpack
#define MALI_WRITE_VALUE_JOB_SECTION_HEADER_print MALI_JOB_HEADER_print
#define MALI_WRITE_VALUE_JOB_SECTION_HEADER_OFFSET 0
#define MALI_WRITE_VALUE_JOB_SECTION_PAYLOAD_TYPE struct MALI_WRITE_VALUE_JOB_PAYLOAD
#define MALI_WRITE_VALUE_JOB_SECTION_PAYLOAD_header MALI_WRITE_VALUE_JOB_PAYLOAD_header
#define MALI_WRITE_VALUE_JOB_SECTION_PAYLOAD_pack MALI_WRITE_VALUE_JOB_PAYLOAD_pack
#define MALI_WRITE_VALUE_JOB_SECTION_PAYLOAD_unpack MALI_WRITE_VALUE_JOB_PAYLOAD_unpack
#define MALI_WRITE_VALUE_JOB_SECTION_PAYLOAD_print MALI_WRITE_VALUE_JOB_PAYLOAD_print
#define MALI_WRITE_VALUE_JOB_SECTION_PAYLOAD_OFFSET 32

#endif
