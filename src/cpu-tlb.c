/*
 * Copyright (C) 2024 pdnguyen of the HCMC University of Technology
 */
/*
 * Source Code License Grant: Authors hereby grants to Licensee 
 * a personal to use and modify the Licensed Source Code for 
 * the sole purpose of studying during attending the course CO2018.
 */
//#ifdef CPU_TLB
/*
 * CPU TLB
 * TLB module cpu/cpu-tlb.c
 */
 
#include "mm.h"
#include <stdlib.h>
#include <stdio.h>

int tlb_change_all_page_tables_of(struct pcb_t *proc,  struct memphy_struct * mp)
{
  /* TODO update all page table directory info 
   *      in flush or wipe TLB (if needed)
   */

  return 0;
}

int tlb_flush_tlb_of(struct pcb_t *proc, struct memphy_struct * mp)
{
  /* TODO flush tlb cached*/

  return 0;
}

/*tlballoc - CPU TLB-based allocate a region memory
 *@proc:  Process executing the instruction
 *@size: allocated size 
 *@reg_index: memory region ID (used to identify variable in symbole table)
 */
int tlballoc(struct pcb_t *proc, uint32_t size, uint32_t reg_index) {
    int addr, val;

    /* Cấp phát vùng nhớ thông qua hàm __alloc */
    val = __alloc(proc, 0, reg_index, size, &addr);

    if (val == 0) { // Nếu cấp phát thành công
        /* Tính toán frame number cho trang mới được cấp phát */
        int frame_num = addr / PAGING_PAGESZ; // Tính frame number bằng cách chia địa chỉ cho kích thước của mỗi trang

        /* Cập nhật TLB Cache với frame number của trang mới được cấp phát */
        tlb_cache_write(proc->tlb, proc->pid, addr / PAGING_PAGESZ, frame_num);
    }

    return val;
}

/*pgfree - CPU TLB-based free a region memory
 *@proc: Process executing the instruction
 *@size: allocated size 
 *@reg_index: memory region ID (used to identify variable in symbole table)
 */
int tlbfree_data(struct pcb_t *proc, uint32_t reg_index) {
    // Giải phóng vùng nhớ của trang
    __free(proc, 0, reg_index);

    // Lấy địa chỉ bắt đầu của trang được giải phóng từ địa chỉ của biến đã giải phóng
    addr_t start_addr = proc->regs[reg_index];

    // Tính toán trang số (page number) của trang được giải phóng
    int page_num = start_addr / PAGING_PAGESZ;

    // Cập nhật TLB Cache để xóa thông tin của trang đã giải phóng
    tlb_cache_write(proc->tlb, proc->pid, page_num, -1);

    // Trả về 0 để biểu thị thành công
    return 0;
}


/*tlbread - CPU TLB-based read a region memory
 *@proc: Process executing the instruction
 *@source: index of source register
 *@offset: source address = [source] + [offset]
 *@destination: destination storage
 */
int tlbread(struct pcb_t * proc, uint32_t source,
            uint32_t offset, 	uint32_t destination) 
{
  BYTE data;
    int frmnum = -1; // Định dạng của frmnum sửa đổi từ BYTE thành int

    // Lấy số trang (page number) từ địa chỉ nguồn
    int page_num = (proc->regs[source] + offset) / PAGING_PAGESZ;

    // Lấy thông tin trang từ TLB Cache
    frmnum = tlb_cache_read(proc->tlb, proc->pid, page_num, data);

#ifdef IODUMP
    if (frmnum >= 0)
        printf("TLB hit at read region=%d offset=%d\n", source, offset);
    else 
        printf("TLB miss at read region=%d offset=%d\n", source, offset);
#ifdef PAGETBL_DUMP
    print_pgtbl(proc, 0, -1); // In ra bảng trang
#endif
    MEMPHY_dump(proc->mram); // In ra dữ liệu trong memory
#endif

    int val = __read(proc, 0, source, offset, &data);

    destination = (uint32_t)data;

    // Cập nhật TLB Cache với số trang mới được truy cập
    tlb_cache_write(proc->tlb, proc->pid, page_num, frmnum);

    return val;
}

/*tlbwrite - CPU TLB-based write a region memory
 *@proc: Process executing the instruction
 *@data: data to be wrttien into memory
 *@destination: index of destination register
 *@offset: destination address = [destination] + [offset]
 */
int tlbwrite(struct pcb_t * proc, BYTE data,
             uint32_t destination, uint32_t offset)
{
  int val;
    BYTE frmnum = -1;

    // Lấy số trang (page number) từ địa chỉ đích
    int page_num = (proc->regs[destination] + offset) / PAGING_PAGESZ;

    // Lấy thông tin trang từ TLB Cache
    frmnum = tlb_cache_read(proc->tlb, proc->pid, page_num, data);

#ifdef IODUMP
    if (frmnum >= 0)
        printf("TLB hit at write region=%d offset=%d value=%d\n", destination, offset, data);
    else
        printf("TLB miss at write region=%d offset=%d value=%d\n", destination, offset, data);
#ifdef PAGETBL_DUMP
    print_pgtbl(proc, 0, -1); // In ra bảng trang
#endif
    MEMPHY_dump(proc->mram); // In ra dữ liệu trong memory
#endif

    val = __write(proc, 0, destination, offset, data);

    // Cập nhật TLB Cache với thông tin mới của trang đã được ghi
    tlb_cache_write(proc->tlb, proc->pid, page_num, frmnum);

    return val;
}

//#endif
