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

#define LOOKUPMETHOD 2

extern thread_ctx_t *threads_ctx;

std::unordered_map<uint64_t, tag_t> value_map;

void dump_value_map(){
    for(const auto& n: value_map)
        printf("{%lu: %d}\n", n.first, n.second);
}

// PIN_FAST_ANALYSIS_CALL
void tagmap_setb(ADDRINT addr, tag_t const &tag) {
    value_map[(uint64_t)*(uint8_t*)addr] = tag;
    printf("setb vm[%lu]=%d (%lu)\n", (uint64_t)*(uint8_t*)addr, tag, value_map.size());
}

void tagmap_setw(ADDRINT addr, tag_t const &tag) {
    value_map[(uint64_t)*(uint16_t*)addr] = tag;
    printf("setw vm[%lu]=%d (%lu)\n", (uint64_t)*(uint16_t*)addr, tag, value_map.size());
}

void tagmap_setl(ADDRINT addr, tag_t const &tag) {
    value_map[(uint64_t)*(uint32_t*)addr] = tag;
    printf("setl vm[%lu]=%d (%lu)\n", (uint64_t)*(uint32_t*)addr, tag, value_map.size());
}

void tagmap_setq(ADDRINT addr, tag_t const &tag) {
    value_map[*(uint64_t*)addr] = tag;
    printf("setq vm[%lu]=%d (%lu)\n", *(uint64_t*)addr, tag, value_map.size());
}

void tagmap_setb_reg(ADDRINT addr, tag_t const &tag) {
    value_map[(uint64_t)addr] = tag;
    printf("setb_reg vm[%lu]=%d (%lu)\n", (uint64_t)addr, tag, value_map.size());
}

void tagmap_setw_reg(ADDRINT addr, tag_t const &tag) {
    value_map[(uint64_t)addr] = tag;
    printf("setw_reg vm[%lu]=%d (%lu)\n", (uint64_t)addr, tag, value_map.size());
}

void tagmap_setl_reg(ADDRINT addr, tag_t const &tag) {
    value_map[(uint64_t)addr] = tag;
    printf("setl_reg vm[%lu]=%d (%lu)\n", (uint64_t)addr, tag, value_map.size());
}

void tagmap_setq_reg(ADDRINT addr, tag_t const &tag) {
    value_map[(uint64_t)addr] = tag;
    printf("setq_reg vm[%lu]=%d (%lu)\n", (uint64_t)addr, tag, value_map.size());
}

tag_t tagmap_getb(ADDRINT addr) {
    //printf("tagmap_getb\n");
    std::unordered_map<uint64_t, tag_t>::const_iterator got = value_map.find((uint64_t)*(uint8_t*)addr);
    if(got == value_map.end()) return tag_traits<tag_t>::cleared_val;
    //printf("finished getb\n");
    return got->second;
}

tag_t tagmap_getw(ADDRINT addr) {
    //printf("tagmap_getw\n");
    std::unordered_map<uint64_t, tag_t>::const_iterator got = value_map.find((uint64_t)*(uint16_t*)addr);
    if(got == value_map.end()) return tag_traits<tag_t>::cleared_val;
    return got->second;
}

tag_t tagmap_getl(ADDRINT addr) {
    //printf("tagmap_getl\n");
    std::unordered_map<uint64_t, tag_t>::const_iterator got = value_map.find((uint64_t)*(uint32_t*)addr);
    if(got == value_map.end()) return tag_traits<tag_t>::cleared_val;
    return got->second;
}

tag_t tagmap_getq(ADDRINT addr) {
    //printf("tagmap_getq\n");
    std::unordered_map<uint64_t, tag_t>::const_iterator got = value_map.find(*(uint64_t*)addr);
    if(got == value_map.end()){
        printf("returning cleared_val\n");
        return tag_traits<tag_t>::cleared_val;
    }
    printf("k=%lu, v=%d\n", got->first, got->second);
    return got->second;
}

tag_t tagmap_getb_reg(ADDRINT addr) {
    //printf("tagmap_getb\n");
    std::unordered_map<uint64_t, tag_t>::const_iterator got = value_map.find((uint64_t)addr);
    if(got == value_map.end()) return tag_traits<tag_t>::cleared_val;
    //printf("finished getb\n");
    return got->second;
}

tag_t tagmap_getw_reg(ADDRINT addr) {
    //printf("tagmap_getw\n");
    std::unordered_map<uint64_t, tag_t>::const_iterator got = value_map.find((uint64_t)addr);
    if(got == value_map.end()) return tag_traits<tag_t>::cleared_val;
    return got->second;
}

tag_t tagmap_getl_reg(ADDRINT addr) {
    //printf("tagmap_getl\n");
    std::unordered_map<uint64_t, tag_t>::const_iterator got = value_map.find((uint64_t)addr);
    if(got == value_map.end()) return tag_traits<tag_t>::cleared_val;
    return got->second;
}

tag_t tagmap_getq_reg(ADDRINT addr) {
    //printf("tagmap_getq\n");
    std::unordered_map<uint64_t, tag_t>::const_iterator got = value_map.find((uint64_t)addr);
    if(got == value_map.end()) return tag_traits<tag_t>::cleared_val;
    return got->second;
}

tag_t tagmap_getn_reg(ADDRINT addr, unsigned int n) {
    //printf("tagmap_getn_reg\n");
    switch(n){
    case 1:
        return tagmap_getb_reg(addr);
        break;
    case 2:
        return tagmap_getw_reg(addr);
        break;
    case 4:
        return tagmap_getl_reg(addr);
        break;
    case 8:
        return tagmap_getq_reg(addr);
        break;
    default:
        printf("CAN'T HANDLE tagmap_getn_reg with strange n\n");
        return tagmap_getb_reg(addr);
    }
}

#if LOOKUPMETHOD == 1
// Issue seems to be with this implementation. Not entirely sure why. 
tag_t tagmap_getn(ADDRINT addr, unsigned int n) {
    //printf("tagmap_getn\n");
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
        ADDRINT i;
        tag_t ts = tag_traits<tag_t>::cleared_val;
        for(i = addr; i + 8 < addr + n; i += 8){
            const tag_t t = tagmap_getq(i);
            if(tag_is_empty(t))
                continue;
            ts = tag_combine(ts, t);
        }
        for(; i + 4 < addr + n; i += 4){
            const tag_t t = tagmap_getl(i);
            if(tag_is_empty(t))
                continue;
            ts = tag_combine(ts, t);
        }
        for(; i + 2 < addr + n; i += 2){
            const tag_t t = tagmap_getw(i);
            if(tag_is_empty(t))
                continue;
            ts = tag_combine(ts, t);
        }
        for(; i < addr + n; i++){
            const tag_t t = tagmap_getb(i);
            if(tag_is_empty(t))
                continue;
            ts = tag_combine(ts, t);
        }
        return ts;
    }
}

void PIN_FAST_ANALYSIS_CALL tagmap_clrn(ADDRINT addr, UINT32 n) {
    //printf("tagmap_clrn\n");
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
        for(i = addr; i + 8 <= addr + n; i += 8){
            tagmap_clrq(i);
        }
        for(; i + 4 <= addr + n; i += 4){
            tagmap_clrl(i);
        }
        for(; i + 2 <= addr + n; i += 2){
            tagmap_clrw(i);
        }
        for(; i <= addr + n; i++){
            tagmap_clrb(i);
        }
    }
}

void PIN_FAST_ANALYSIS_CALL tagmap_setn(ADDRINT addr, UINT32 n, tag_t const &tag) {
    printf("tagmap_setn(): LOOKUPMETHOD=%d, n=%u\n", LOOKUPMETHOD, n);
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
        for(i = addr; i + 8 < addr + n; i += 8){
            tagmap_setq(i, tag);
        }
        for(; i + 4 < addr + n; i += 4){
            tagmap_setl(i, tag);
        }
        for(; i + 2 < addr + n; i += 2){
            tagmap_setw(i, tag);
        }
        for(; i < addr + n; i++){
            tagmap_setb(i, tag);
        }
    }
}

#elif LOOKUPMETHOD == 2
tag_t tagmap_getn(ADDRINT addr, unsigned int n) {
    //printf("tagmap_getn\n");
    ADDRINT i;
    tag_t ts = tag_traits<tag_t>::cleared_val;
    if(n >= 8){
        for(i = addr; i + 8 <= addr + n; i++){
            const tag_t t = tagmap_getq(i);
            if(tag_is_empty(t))
                continue;
            ts = tag_combine(ts, t);
        }
    } else if(n >= 4){
        for(i = addr; i + 4 <= addr + n; i++){
            const tag_t t = tagmap_getl(i);
            if(tag_is_empty(t))
                continue;
            ts = tag_combine(ts, t);
        }
    } else if(n >= 2){
        for(i = addr; i + 2 <= addr + n; i++){
            const tag_t t = tagmap_getw(i);
            if(tag_is_empty(t))
                continue;
            ts = tag_combine(ts, t);
        }
    } else {
        ts = tagmap_getb(addr);
    }
    return ts;
}

void PIN_FAST_ANALYSIS_CALL tagmap_clrn(ADDRINT addr, UINT32 n) {
    //printf("tagmap_clrn\n");
    ADDRINT i;
    if(n >= 8){
        for(i = addr; i + 8 <= addr + n; i++){
            tagmap_clrq(i);
        }
    } else if(n >= 4){
        for(i = addr; i + 4 <= addr + n; i++){
            tagmap_clrl(i);
        }
    } else if(n >= 2){
        for(i = addr; i + 2 <= addr + n; i++){
            tagmap_clrw(i);
        }
    } else {
        tagmap_clrb(addr);
    }
}

void PIN_FAST_ANALYSIS_CALL tagmap_setn(ADDRINT addr, UINT32 n, tag_t const &tag) {
    printf("tagmap_setn(): LOOKUPMETHOD=%d, n=%u\n", LOOKUPMETHOD, n);
    //printf("tagmap_clrn\n");
    ADDRINT i;
    if(n >= 8){
        for(i = addr; i + 8 <= addr + n; i++){
            tagmap_setq(i, tag);
        }
    } else if(n >= 4){
        for(i = addr; i + 4 <= addr + n; i++){
            tagmap_setl(i, tag);
        }
    } else if(n >= 2){
        for(i = addr; i + 2 <= addr + n; i++){
            tagmap_setw(i, tag);
        }
    } else {
        tagmap_setb(addr, tag);
    }
}

#else
tag_t tagmap_getn(ADDRINT addr, unsigned int n) {
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

void PIN_FAST_ANALYSIS_CALL tagmap_clrn(ADDRINT addr, UINT32 n) {
  ADDRINT i;
  for (i = addr; i < addr + n; i++) {
    tagmap_clrb(i);
  }
}

void PIN_FAST_ANALYSIS_CALL tagmap_setn(ADDRINT addr, UINT32 n, tag_t const &tag) {
  printf("tagmap_setn(): LOOKUPMETHOD=%d, n=%u\n", LOOKUPMETHOD, n);
  ADDRINT i;
  for (i = addr; i < addr + n; i++) {
    tagmap_setb(i, tag);
  }
  dump_value_map();
}

#endif

void PIN_FAST_ANALYSIS_CALL tagmap_clrb(ADDRINT addr) {
    //printf("tagmap_clrb\n");
    value_map.erase((uint64_t)*(uint8_t*)addr);
}

void PIN_FAST_ANALYSIS_CALL tagmap_clrw(ADDRINT addr) {
    //printf("tagmap_clrw\n");
    value_map.erase((uint64_t)*(uint16_t*)addr);
}

void PIN_FAST_ANALYSIS_CALL tagmap_clrl(ADDRINT addr) {
    //printf("tagmap_clrl\n");
    value_map.erase((uint64_t)*(uint32_t*)addr);
}

void PIN_FAST_ANALYSIS_CALL tagmap_clrq(ADDRINT addr) {
    //printf("tagmap_clrq\n");
    value_map.erase(*(uint64_t*)addr);
}
