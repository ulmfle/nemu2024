#include "common.h"
#include <sys/ioctl.h>
#include <string.h>

typedef struct {
	char *name;
	uint32_t size;
	uint32_t disk_offset;
} file_info;

enum {SEEK_SET, SEEK_CUR, SEEK_END};

/* This is the information about all files in disk. */
static const file_info file_table[] __attribute__((used)) = {
	{"1.rpg", 188864, 1048576}, {"2.rpg", 188864, 1237440},
	{"3.rpg", 188864, 1426304}, {"4.rpg", 188864, 1615168},
	{"5.rpg", 188864, 1804032}, {"abc.mkf", 1022564, 1992896},
	{"ball.mkf", 134704, 3015460}, {"data.mkf", 66418, 3150164},
	{"desc.dat", 16027, 3216582}, {"fbp.mkf", 1128064, 3232609},
	{"fire.mkf", 834728, 4360673}, {"f.mkf", 186966, 5195401},
	{"gop.mkf", 11530322, 5382367}, {"map.mkf", 1496578, 16912689},
	{"mgo.mkf", 1577442, 18409267}, {"m.msg", 188232, 19986709},
	{"mus.mkf", 331284, 20174941}, {"pat.mkf", 8488, 20506225},
	{"rgm.mkf", 453202, 20514713}, {"rng.mkf", 4546074, 20967915},
	{"sss.mkf", 557004, 25513989}, {"voc.mkf", 1997044, 26070993},
	{"wor16.asc", 5374, 28068037}, {"wor16.fon", 82306, 28073411},
	{"word.dat", 5650, 28155717},
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

int fs_ioctl(int fd, uint32_t request, void *p) {
	assert(request == TCGETS);
	return (fd >= 0 && fd <= 2 ? 0 : -1);
}

void ide_read(uint8_t *, uint32_t, uint32_t);
void ide_write(uint8_t *, uint32_t, uint32_t);
/* TODO: implement a simplified file system here. */

struct Fstate {
	bool opened;
	uint32_t offset;
} fstate[NR_FILES + 3];

int fs_open(const char *pathname, int flags) {
	int idx;
	for (idx = 3; idx < NR_FILES + 3; ++idx) {
		if (strcmp(file_table[idx - 3].name, pathname) == 0) {
			fstate[idx].opened = true;
			fstate[idx].offset = 0;
			break;
		}
	}

	assert(idx != NR_FILES + 3);
	return idx;
}

int fs_read(int fd, void *buf, int len) {
	if (!fstate[fd].opened) return -1;
	int of = fstate[fd].offset + len - file_table[fd - 3].size;
	len -= (of > 0 ? of : 0);
	ide_read(buf, fstate[fd].offset, len);
	fstate[fd].offset += len;
	return len;
}

int fs_write(int fd, void *buf, int len) {
	if (!fstate[fd].opened) return -1;
	int of = fstate[fd].offset + len - file_table[fd - 3].size;
	len -= (of > 0 ? of : 0);
	ide_write(buf, fstate[fd].offset, len);
	fstate[fd].offset += len;
	return len;
}

int fs_lseek(int fd, int offset, int whence) {
	if (!fstate[fd].opened) return -1;
	int res = fstate[fd].offset;
	switch (whence) {
		case SEEK_SET:
			res = offset;
			break;
		case SEEK_CUR:
			res += offset;
			break;
		case SEEK_END:
			res = offset + file_table[fd - 3].size;
			break;
		default:
			return -1;
	}

	fstate[fd].offset = res >= file_table[fd - 3].size ? file_table[fd - 3].size - 1 : res;
	return fstate[fd].offset;
}

int fs_close(int fd) {
	if (!fstate[fd].opened) return -1;
	fstate[fd].opened = false;
	fstate[fd].offset = 0;
	return 0;
}