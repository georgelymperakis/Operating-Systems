/*
 * help.h
 *
 * A set of helper functions for retrieving or printing information related to
 * the virtual memory of a process.
 *
 * Operating Systems course, CSLab, ECE, NTUA
 *
 */

#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <unistd.h>

/******************************************************************************
 * Helper Functions
 */

/*
 * Print the given message and exit with a failed status.
 */
#define die(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

/*
 * Retrieve the system's page size.
 */
long get_page_size(void);

/*
 * A function that prints the process's virtual memory map.
 * It prints the populated virtual memory areas of the process, their
 * permissions, and the name of the file they map, if any.
 *
 * For further details, see `/proc/[pid]/maps` in proc(5) (or online at:
 * https://man7.org/linux/man-pages/man5/proc.5.html).
 */
void show_maps(void);

/*
 * Search maps for a specific VA.
 * If populated, print to stdout the virtual address area (VMA) it belongs to,
 * along with its permissions, etc.
 */
void show_va_info(uint64_t va);

/*
 * A function that receives a virtual address (VA) as an argument and, if it is
 * mapped, it returns the physical address (PA) that it maps to.
 * If the VA is not mapped, it returns 0.
 *
 * For further details, see `/proc/[pid]/pagemap` in proc(5) (or online at:
 * https://man7.org/linux/man-pages/man5/proc.5.html), as well as the Linux
 * documentation at: https://www.kernel.org/doc/Documentation/vm/pagemap.txt.
 */
uint64_t get_physical_address(unsigned long virt_addr);

void press_enter(void);

#endif /* MAP_H */

