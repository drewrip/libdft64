/*-
 * Copyright (c) 2010, Columbia University
 * All rights reserved.
 *
 * This software was developed by Vasileios P. Kemerlis <vpk@cs.columbia.edu>
 * at Columbia University, New York, NY, USA, in June 2010.
 *
 * Georgios Portokalidis <porto@cs.columbia.edu> contributed to the
 * optimized implementation of tagmap_setn() and tagmap_clrn()
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Columbia University nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "tagmap.h"
#include "branch_pred.h"
#include "debug.h"
#include "libdft_api.h"
#include "pin.H"
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>

extern thread_ctx_t *threads_ctx;

std::unordered_map<uint8_t, tag_t> value_map8;
std::unordered_map<uint16_t, tag_t> value_map16;
std::unordered_map<uint32_t, tag_t> value_map32;
std::unordered_map<uint64_t, tag_t> value_map64;

// PIN_FAST_ANALYSIS_CALL
void tagmap_setb(ADDRINT addr, tag_t const &tag) {
    printf("tagmap_setb(): addr=%lx, value=%d\n", addr, *(uint8_t*)Addrint2VoidStar(addr));
    value_map8[*(uint8_t*)Addrint2VoidStar(addr)] = tag; // Running out of shared memory!
    printf("finished tagmap_setb: size=%zu\n", value_map8.size());
}

void tagmap_setw(ADDRINT addr, tag_t const &tag) {
    printf("tagmap_setw()\n");
    value_map16[*(uint16_t*)Addrint2VoidStar(addr)] = tag;
}

void tagmap_setl(ADDRINT addr, tag_t const &tag) {
    printf("tagmap_setl()\n");
    value_map32[*(uint32_t*)Addrint2VoidStar(addr)] = tag;
}

void tagmap_setq(ADDRINT addr, tag_t const &tag) {
    printf("tagmap_setq()\n");
    value_map64[*(uint64_t*)Addrint2VoidStar(addr)] = tag;
}

void PIN_FAST_ANALYSIS_CALL tagmap_setn(ADDRINT addr, UINT32 n, tag_t const &tag) {
    printf("tagmap_setn()\n");
    switch(n){
    case 1:
        tagmap_setb(addr, tag);
        break;
    case 2:
        tagmap_setw(addr, tag);
        break;
    case 4:
        tagmap_setl(addr, tag);
        break;
    case 8:
        tagmap_setq(addr, tag);
        break;
    default:
        ADDRINT i;
        for (i = addr; i < addr + n; i++) {
            tagmap_setb(i, tag);
        }
    }
}

void tagmap_setb_reg(THREADID tid, unsigned int reg_idx, unsigned int off,
                     tag_t const &tag) {
    printf("tagmap_setb_reg()\n");
    threads_ctx[tid].vcpu.gpr[reg_idx][off] = tag;
}

tag_t tagmap_getb(ADDRINT addr) { return value_map8[*(uint8_t*)Addrint2VoidStar(addr)]; }

tag_t tagmap_getw(ADDRINT addr) { return value_map16[*(uint16_t*)Addrint2VoidStar(addr)]; }

tag_t tagmap_getl(ADDRINT addr) { return value_map32[*(uint32_t*)Addrint2VoidStar(addr)]; }

tag_t tagmap_getq(ADDRINT addr) { return value_map64[*(uint64_t*)Addrint2VoidStar(addr)]; }

tag_t tagmap_getb_reg(THREADID tid, unsigned int reg_idx, unsigned int off) {
    printf("tagmap_getb_reg()\n");
  return threads_ctx[tid].vcpu.gpr[reg_idx][off];
}

tag_t tagmap_getn(ADDRINT addr, unsigned int n) {
    printf("tagmap_getn: address=%zu, n=%u\n", addr, n);
    switch(n){
    case 1:
        return tagmap_getb(addr);
        break;
    case 2:
        return tagmap_getw(addr);
        break;
    case 4:
        return tagmap_getl(addr);
        break;
    case 8:
        return tagmap_getq(addr);
        break;
    default:
        tag_t ts = tag_traits<tag_t>::cleared_val;
        for (size_t i = 0; i < n; i++) {
            const tag_t t = tagmap_getb(addr + i);
            if (tag_is_empty(t))
                continue;
            // LOGD("[tagmap_getn] %lu, ts: %d, %s\n", i, ts, tag_sprint(t).c_str());
            ts = tag_combine(ts, t);
            // LOGD("t: %d, ts:%d\n", t, ts);
        }
        return ts;
    }
}

tag_t tagmap_getn_reg(THREADID tid, unsigned int reg_idx, unsigned int n) {
    printf("tagmap_getn_reg()\n");
  tag_t ts = tag_traits<tag_t>::cleared_val;
  for (size_t i = 0; i < n; i++) {
    const tag_t t = tagmap_getb_reg(tid, reg_idx, i);
    if (tag_is_empty(t))
      continue;
    // LOGD("[tagmap_getn] %lu, ts: %d, %s\n", i, ts, tag_sprint(t).c_str());
    ts = tag_combine(ts, t);
    // LOGD("t: %d, ts:%d\n", t, ts);
  }
  return ts;
}

void PIN_FAST_ANALYSIS_CALL tagmap_clrb(ADDRINT addr) {
    value_map8.erase(*(uint8_t*)Addrint2VoidStar(addr));
}

void PIN_FAST_ANALYSIS_CALL tagmap_clrw(ADDRINT addr) {
    printf("tagmap_clrw()\n");
    value_map16.erase(*(uint16_t*)Addrint2VoidStar(addr));
}

void PIN_FAST_ANALYSIS_CALL tagmap_clrl(ADDRINT addr) {
    printf("tagmap_clrl()\n");
    value_map32.erase(*(uint32_t*)Addrint2VoidStar(addr));
}

void PIN_FAST_ANALYSIS_CALL tagmap_clrq(ADDRINT addr) {
    printf("tagmap_clrq()\n");
    value_map64.erase(*(uint64_t*)Addrint2VoidStar(addr));
}

void PIN_FAST_ANALYSIS_CALL tagmap_clrn(ADDRINT addr, UINT32 n) {
    printf("tagmap_clrn()\n");
    switch(n){
    case 1:
        tagmap_clrb(addr);
        break;
    case 2:
        tagmap_clrw(addr);
        break;
    case 4:
        tagmap_clrl(addr);
        break;
    case 8:
        tagmap_clrq(addr);
        break;
    default:
        ADDRINT i;
        for (i = addr; i < addr + n; i++) {
            tagmap_clrb(i);
        }
    }
}
