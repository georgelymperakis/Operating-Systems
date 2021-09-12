/*
 * help.c
 *
 * A set of helper functions for retrieving or printing information related to
 * the virtual memory of a process.
 *
 * Operating Systems course, CSLab, ECE, NTUA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>

#include "help.h"

#define LINELEN          256
#define PAGEMAP_PATH     "/proc/self/pagemap"
#define MAPS_PATH        "/proc/self/maps"

#define PFN_MASK         0x7FFFFFFFFFFFFF
#define GET_PFN(X)       ((X) & PFN_MASK)
#define GET_BIT(X, nr)   (((X) >> (nr)) & (uint64_t)1)


/*
 * Retrieve the system's page size.
 */
long get_page_size(void)
{
	static long page_size = 0;

	if (0 == page_size && -1 == (page_size = sysconf(_SC_PAGESIZE)))
		die("sysconf(_SC_PAGESIZE)");

	return page_size;
}

/*
 * A function that prints the process's virtual memory map.
 * It prints the populated virtual memory areas of the process, their
 * permissions, and the name of the file they map, if any.
 *
 * For further details, see `/proc/[pid]/maps` in proc(5) (or online at:
 * https://man7.org/linux/man-pages/man5/proc.5.html).
 */
void show_maps(void)
{
	FILE *f;
	char line[LINELEN];

	f = fopen(MAPS_PATH, "r");
	if (!f) {
		printf("Cannot open " MAPS_PATH ": %s\n", strerror(errno));
		return;
	}

	printf("\nVirtual Memory Map of process [%ld]:\n", (long)getpid());
	while (fgets(line, LINELEN, f) != NULL) {
		printf("%s", line);
	}
	printf("--------------------------------------------------------\n\n");

	if (0 != fclose(f))
		perror("fclose(" MAPS_PATH ")");
}

/*
 * Search maps for a specific VA.
 * If populated, print to stdout the virtual address area (VMA) it belongs to,
 * along with its permissions, etc.
 */
void show_va_info(uint64_t va)
{
	FILE *f;
	char line[LINELEN];
	int n;
	uint64_t vm_start, vm_end;

	f = fopen(MAPS_PATH, "r");
	if (!f) {
		printf("Cannot open " MAPS_PATH ": %s\n", strerror(errno));
		return;
	}

	while (fgets(line, LINELEN, f) != NULL) {
		n = sscanf(line, "%lX-%lX", &vm_start, &vm_end);
		if (n != 2) {
			printf("Invalid line read from %s (6)\n", line);
			continue;
		}

		if (va >= vm_start && va < vm_end) {
			printf("%s", line);
			goto out;
		}
	}

	printf("VA is not allocated!\n");
out:
	if (0 != fclose(f))
		perror("fclose(" MAPS_PATH ")");
}

/*
 * A function that receives a virtual address (VA) as an argument and, if it is
 * mapped, it returns the physical address (PA) that it maps to.
 * If the VA is not mapped, it returns 0.
 *
 * For further details, see `/proc/[pid]/pagemap` in proc(5) (or online at:
 * https://man7.org/linux/man-pages/man5/proc.5.html), as well as the Linux
 * documentation at: https://www.kernel.org/doc/Documentation/vm/pagemap.txt.
 */
uint64_t get_physical_address(uint64_t va)
{
	int pmfd = 0;
	uint64_t vpn = 0, offset = 0, pagemap_entry = 0;

	// Calculate the virtual page number and the offset for the given VA.
	vpn = va / get_page_size();
	offset = va % get_page_size();

	// Open `/proc/self/pagemap`...
	if (-1 == (pmfd = open(PAGEMAP_PATH, O_RDONLY)))
		die("open(" PAGEMAP_PATH ")");
	// ...seek to the entry that refers to the VA we are interested in...
	if ((off_t)-1 == lseek(pmfd, vpn * sizeof(pagemap_entry), SEEK_SET))
		die("lseek(" PAGEMAP_PATH ")");
	// ...and read the entry.
	if (-1 == read(pmfd, &pagemap_entry, sizeof(pagemap_entry)))
		die("read(" PAGEMAP_PATH ")");
	if (-1 == close(pmfd))
		perror("close(" PAGEMAP_PATH ")");

	// Then, check the value of the `PM_PRESENT` bit...
	if (GET_BIT(pagemap_entry, 63))
		// ...and if it is found to be set, return the corresponding PA
		return GET_PFN(pagemap_entry) * get_page_size() + offset;
	// ...otherwise announce its absence and return 0.
	printf("VA[0x%lx] is not mapped; no physical memory allocated.\n", va);
	return 0;
}

void press_enter(void)
{
	char enter = 0;

	while (enter != '\r' && enter != '\n') {
		enter = getchar();
	}
}

