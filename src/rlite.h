#ifndef _RLITE_H
#define _RLITE_H

#include <stdio.h>
#include "status.h"
#include "btree.h"

#define RL_MEMORY_DRIVER 0
#define RL_FILE_DRIVER 1

#define RLITE_OPEN_READONLY  0x00000001
#define RLITE_OPEN_READWRITE 0x00000002
#define RLITE_OPEN_CREATE    0x00000004

struct rlite;

typedef struct rl_data_type {
	const char *name;
	int (*serialize)(struct rlite *db, void *obj, unsigned char *data);
	int (*deserialize)(struct rlite *db, void **obj, void *context, unsigned char *data);
	int (*destroy)(struct rlite *db, void *obj);
} rl_data_type;

typedef struct {
	FILE *fp;
	char *filename;
	int mode;
} rl_file_driver;

typedef struct {
	long page_number;
	rl_data_type *type;
	void *obj;
} rl_page;

typedef struct rlite {
	long number_of_pages;
	long next_empty_page;
	long page_size;
	void *driver;
	int driver_type;
	long read_pages_alloc;
	long read_pages_len;
	rl_page **read_pages;
	long write_pages_alloc;
	long write_pages_len;
	rl_page **write_pages;
} rlite;

int rl_open(const char *filename, rlite **db, int flags);
int rl_close(rlite *db);

int rl_read_header(rlite *db);
int rl_read(struct rlite *db, rl_data_type *type, long page, void *context, void **obj);
int rl_write(struct rlite *db, rl_data_type *type, long page, void *obj);
int rl_delete(struct rlite *db, long page);
int rl_commit(struct rlite *db);
int rl_discard(struct rlite *db);

int rl_set_key(rlite *db, const char *key, long keylen, long value);
int rl_get_key(rlite *db, const char *key, long keylen, long *value);

extern rl_data_type rl_data_type_header;
extern rl_data_type rl_data_type_btree_hash_md5_long;
extern rl_data_type rl_data_type_btree_node_hash_md5_long;
extern rl_data_type rl_data_type_btree_set_long;
extern rl_data_type rl_data_type_btree_node_set_long;
extern rl_data_type rl_data_type_btree_hash_long_long;
extern rl_data_type rl_data_type_btree_node_hash_long_long;

#endif
