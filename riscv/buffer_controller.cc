#include "mmu.h"
#include "buffer_controller.h"
#include <iostream>
using namespace std;

buffer_controller_t::buffer_controller_t(mmu_t * _mmu, unsigned int sz)
{
    mmu = _mmu;
    buffer_sz = sz;
    buffer = new uint8_t [buffer_sz];
    for (unsigned int i=0;i<sz; i++) buffer[i]=0xdd;
    buffer_rd_idx = 0;

    start=0;
    col_idx=0;
}

buffer_controller_t::~buffer_controller_t()
{
    if (buffer) delete [] buffer;
    buffer = 0;
}

uint8_t * buffer_controller_t::read32(reg_t addr)
{
    uint8_t * p;
    if (addr >= CONFIG_RANGE)
    {
        p = (uint8_t*) &buffer[buffer_rd_idx];
        // here is where the buffer controller must 
        // fill data into buffer from the vector using v[cols[col_idx]]
        // We fetch v[.] from the host via the MMU
        // this is two reads - once to read from cols[col_idx].
        // and using this, to read from v[.]
        reg_t v_idx = col_base + 4*col_idx++;
        reg_t val;
        mmu->load_slow_path(v_idx, 4, (uint8_t*)&val); 

        reg_t v_addr = v_base + val*4;
        mmu->load_slow_path(v_addr, 4, p); 
        
        buffer_rd_idx += 4;
        if (buffer_rd_idx == buffer_sz) buffer_rd_idx = 0;
        return p;
    } else {
        switch (addr)
        {
        case OFFSET_N: { return (uint8_t*)&n; }
        case OFFSET_ROWS: { return (uint8_t*) &row_base; }
        case OFFSET_COLS: { return (uint8_t*) &col_base; }
        case OFFSET_V:    { return (uint8_t*) &v_base; }
        case OFFSET_BID:  { return (uint8_t*) &buf_id; }
        case OFFSET_START: { return (uint8_t*) &start; }
        default: return 0; 
        }
    }
}

void buffer_controller_t::write32(reg_t addr, uint8_t * data)
{
    if (addr >= CONFIG_RANGE)
    {
        strncpy((char*)&buffer[buffer_wr_idx], (const char*)data, 4);
        buffer_wr_idx+=4;
        if (buffer_wr_idx == buffer_sz) buffer_wr_idx = 0;
    } else {
        unsigned long val = *(unsigned long*)data;
        switch (addr)
        {
        case OFFSET_N: { n=val; break; }
        case OFFSET_ROWS:{ row_base=val; break; }
        case OFFSET_COLS:{ col_base=val; break; }
        case OFFSET_V:   { v_base=val; break; }
        case OFFSET_BID: { buf_id=val; break; }
        case OFFSET_START: { start=val; break; }
        default: break;
        }
    } 
}
