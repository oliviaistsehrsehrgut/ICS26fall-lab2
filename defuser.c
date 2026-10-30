#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    uint32_t high = (uint32_t)(challenge >> 32);
    uint32_t low  = (uint32_t)challenge;
    uint32_t mid  = (uint32_t)(challenge >> 16);

    uint32_t res2 = (low + 0xf592) ^ high;

    uint32_t res0 = (low * 3) ^ (mid + 0x347e);

    uint32_t final_ans = (res0 + res2 * 2) ^ 0xc649;

    values[0] = final_ans;
    count = 1; 
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    values[0] = (challenge & 0x7f) + 0x17;

    uint32_t shift = 21; 
    for (size_t i = 1; i < 6; ++i) {
        uint32_t prev_mul_5 = values[i - 1] * 5;
        uint32_t part2 = ((challenge >> shift) & 0xf) + i;
        values[i] = prev_mul_5 ^ part2;
        shift += 5;
    }
    count = 6; 
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
        values[0] = challenge & 0x7;

    uint32_t x = (uint32_t)(challenge >> 32) ^ (uint32_t)(challenge >> 3);
    uint32_t ans = 0;

    switch (values[0]) {
        case 0:
            ans = (x * 2) ^ 0x5eb4;
            
            break;
        case 1:
            ans = (x ^ 0x1cdb) + (x >> 1);
            break;
        case 2:
            ans = (x * 4) ^ 0x1ca4;
            
            break;
        case 3:
            ans = (x ^ 0x4c8f) + (x >> 1);
            break;
        case 4:
            ans = (x * 5) + 0x424dc;
            break;
        case 5:
            ans = (x * 3) + 0xb0c9;
            break;
        case 6:
            ans = (x * 5) + 0x888a;
            break;
        case 7:
            ans = (x * 5) + 0x3c53c;
            break;
    }
   
    values[1] = ans;
    count = 2;
    return write_answer(out, cap, values, count);
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    uint32_t A = challenge & 0x7f;
    uint32_t D = (uint32_t)(challenge >> 32) ^ (uint32_t)(challenge >> 7);
    
    uint32_t lo = 0;
    uint32_t hi = 0x7f;

    uint32_t mids[32]; 
    int dirs[32];     
    int depth = 0;

    while (1) {
        uint32_t mid = lo + (hi - lo) / 2;
        
        if (mid == A) {
            break; 
        }
        
        mids[depth] = mid;
        if (A > mid) {
            dirs[depth] = 1; 
            lo = mid + 1;
        } else {
            dirs[depth] = 0; 
            hi = mid - 1;
        }
        depth++;
    }

    uint32_t res = (A + 0x32) ^ D;

    for (int i = depth - 1; i >= 0; i--) {
        uint32_t mid = mids[i];
        if (dirs[i] == 1) {
            res = ((res * 3) ^ (mid + D)) ^ 0x1b;
        } else {
            res = ((res * 3) ^ (mid + D)) ^ 0xf;
        }
    }

    values[0] = res;
    return write_answer(out, cap, values, count);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;

    typedef struct { 
        int next; 
        uint32_t val8; 
        uint32_t val12; 
    } Node;

  
    static const Node nodes[] = {
        { 3,  0x0000222f, 0x0000f8e8 }, // 0: 0x405ba0
        { 10, 0x00000784, 0x0000c61b }, // 1: 0x405bb0
        { 9,  0x00000411, 0x00006274 }, // 2: 0x405bc0
        { 8,  0x0000062a, 0x00002ecf }, // 3: 0x405bd0
        { 5,  0x00000982, 0x0000f929 }, // 4: 0x405be0
        { -1, 0x00000b6a, 0x0000a57b }, // 5: 0x405bf0
        { 2,  0x00000104, 0x0000558c }, // 6: 0x405c00
        { 4,  0x0000034d, 0x00001ef6 }, // 7: 0x405c10
        { 6,  0x00000443, 0x00001bc0 }, // 8: 0x405c20
        { 1,  0x00000542, 0x00003b87 }, // 9: 0x405c30
        { 7,  0x0000081d, 0x00008984 }, // 10: 0x405c40
        { 0,  0x00000a7c, 0x0000dc6c }  // 11: 0x405c50
    };

    static const int heads[] = { 11, 0, 3, 8, 6, 2, 9, 1 };

    uint32_t idx = challenge & 0x7;
    uint32_t rcx_val = challenge >> 3;
    uint32_t steps = rcx_val & 0x3;

    int node_idx = heads[idx];

    uint32_t eax = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);
    eax ^= 0x55a5;
    eax += nodes[node_idx].val12;
    eax = eax * 5;
    eax ^= nodes[node_idx].val8;

    values[1] = nodes[node_idx].val8;

    for (uint32_t i = 1; i <= steps; i++) {
        node_idx = nodes[node_idx].next;
        eax += nodes[node_idx].val12;
        eax = eax * 5;
        eax ^= nodes[node_idx].val8;
        values[1] = nodes[node_idx].val8;
    }

    values[0] = eax;
    return write_answer(out, cap, values, count);
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    uint32_t arg = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);

    uint32_t idx1 = challenge & 0x7;
    uint32_t res1 = 0;
    switch (idx1) {
        case 0: res1 = ((arg * 2) + 0xe83a) ^ (arg * 5); break;
        case 1: res1 = (arg >> 3) | (arg << 29);break;
        case 2: res1 = (arg ^ 0xbc38) + (arg * 8);break;
        case 3: res1 = ((arg >> 1) ^ arg) + 0xe365;break;
        case 4: res1 = ((arg << 3) | (arg >> 29)) ^ 0x5387;break;
        case 5: res1 = ((arg >> 1) ^ arg) + 0xe365;break;
        case 6: res1 = (arg + 0x534b) ^ (arg >> 5);break;
        case 7: res1 = (arg ^ 0xbc38) + (arg * 8);break;
    }

    res1 ^= 0xb8f9;

    uint32_t idx2 = (challenge >> 3) & 0x7;
    uint32_t res2 = 0;
    switch (idx2) {
        case 0: res2 = ((res1 * 2) + 0xe83a) ^ (res1 * 5);break;
        case 1: res2 = (res1 >> 3) | (res1 << 29);break;
        case 2: res2 = (res1 ^ 0xbc38) + (res1 * 8);break;
        case 3: res2 = ((res1 >> 1) ^ res1) + 0xe365;break;
        case 4: res2 = ((res1 << 3) | (res1 >> 29)) ^ 0x5387;break;
        case 5: res2 = ((res1 >> 1) ^ res1) + 0xe365;break;
        case 6: res2 = (res1 + 0x534b) ^ (res1 >> 5);break;
        case 7: res2 = (res1 ^ 0xbc38) + (res1 * 8);break;
    }

    values[0] = res2 + 0x1401;
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    uint64_t C = challenge;
    uint64_t T = context;

    uint32_t v0 = T & 0x3;

    uint32_t C_lo = (uint32_t)C;
    uint32_t C_hi = (uint32_t)(C >> 32);
    uint32_t e_si = C_hi ^ C_lo; 
    uint32_t T_lo = (uint32_t)T;
    uint32_t T_hi = (uint32_t)(T >> 32);
    uint32_t e_r8 = T_hi ^ T_lo; 

    uint32_t r9 = (v0 * 0x7263) ^ (uint32_t)(T >> 16);

    uint32_t mix_arg = e_si ^ e_r8; 
    uint32_t mix_res = (mix_arg << 1) | (mix_arg >> 31); // rol 1
    mix_res += 0x7263;
    mix_res ^= 0xbbd8;

    uint32_t v1 = mix_res ^ r9;

    uint32_t e_si_halved = e_si >> 1;
    uint32_t edx = e_r8 + v0 + 0xbbd8 + e_si_halved;
    uint32_t v2 = v1 ^ edx;

    if (cap <= 32) return 1;

    memset(out, 'A', 32);

    int n = snprintf(out + 32, cap - 32,
                     "OPEN %u %u %u", v0, v1, v2);

    if (n < 0 || (size_t)n >= cap - 32)
        return 1;

    return 0;
}
