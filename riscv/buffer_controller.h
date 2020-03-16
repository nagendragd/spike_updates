#ifndef _BUFF_CTRL_H
#define _BUFF_CTRL_H

#include <stdint.h>
#include "decode.h"

// this class represents our buffer controller

typedef enum {
    OFFSET_N = 0,
    OFFSET_ROWS = 4,
    OFFSET_COLS = 8,
    OFFSET_V = 12,
    OFFSET_BID=16,
    OFFSET_START=24
} MMR_OFFSETS;

class buffer_controller_t
{
private:
    unsigned int buffer_sz;
    uint8_t *buffer;
    unsigned int buffer_rd_idx=0;
    unsigned int buffer_wr_idx=0;
    const unsigned int CONFIG_RANGE = 1024;

    unsigned long n;
    reg_t row_base;
    reg_t col_base;
    reg_t v_base;
    unsigned long buf_id;
    unsigned long start;

    unsigned long col_idx; // state of buffer controller

    mmu_t * mmu;

public:
    buffer_controller_t(mmu_t* mmu, unsigned int sz);
    ~buffer_controller_t();
    uint8_t * read32(reg_t addr);
    void write32(reg_t addr, uint8_t * data);
    
};

#endif
