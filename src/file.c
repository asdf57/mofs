#include "file.h"
#include "utils.h"
#include "logger.h"

const char *op_names[NUM_OPS] = {
	"GETATTR",
	"READLINK",
	"MKNOD",
	"MKDIR",
	"UNLINK",
	"RMDIR",
	"SYMLINK",
	"RENAME",
	"LINK",
	"CHMOD",
	"CHOWN",
	"TRUNCATE",
	"OPEN",
	"READDIR",
	"READ",
	"UTIMENS",
	"CREATE",
};


FSEntry*
add_entry(FSEntry entry) {
	int idx;

	idx = 0;

	if ((idx = get_free_spot()) == -1)
		return NULL;

	entry.id = idx;

	pool[idx] = entry;

	return &pool[idx];
}

void
remove_entry(FSEntry *entry) {
	memset(&pool[entry->id], 0, sizeof(FSEntry));
}

FSEntry *
get_closest_entry(const char *path) {
	int i;
	FSEntry *marker;
	char dup_path[strlen(path)+1];
	char *cur_component;
	char *marker_component;

	logger(INFO, "[lookup_entry] path: %s\n", path);

	marker = &pool[0];

	if (strcmp(path, "/") == 0)
		return marker;

	strcpy(dup_path, path);

	cur_component = strtok(dup_path, "/");
	while (cur_component != NULL) {
		for (i = 0; i < marker->content.dir.num_children; i++) {
			logger(INFO, "[lookup_entry] current child: %s\n\n", marker->content.dir.children[i]->name);
			if (strcmp(cur_component, marker->content.dir.children[i]->name) == 0) {
				marker = marker->content.dir.children[i];
				break;
			}
		}

		if (i+1 == marker->content.dir.num_children && strcmp(marker->name, cur_component) != 0) {
			return marker;
		}

		cur_component = strtok(NULL, "/");
	}

	logger(INFO, "[lookup_entry]: obtained marker: %s\n", marker->name);

	return marker;
}

int
execute_handler(FSEntry *entry, FSop op, int nargs, ...) {
	int res;
	generic_handler handler;
	va_list args;
	va_start(args, nargs);

	logger(INFO, "[execute_handler] Provided entry w/ name %s\n", entry->name);
  logger(INFO, "Specified operation: %s\n", op_names[bitflag_to_idx(op)]);

	if (nargs == 0) {
		va_end(args);
		fprintf(stderr, "no args passed for FUSE handler!\n");
		return -1;
	}

	if (entry->type == MOFS_FILE && op & (MKDIR | RMDIR)) {
		fprintf(stderr, "wrong fs op provided for file!");
		return -1;
	}

	if (op < GETATTR || op > NUM_OPS) {
		fprintf(stderr, "unsupported file system operation (%d)!\n", op);
		return -1;
	}

	handler = (generic_handler) entry->handlers[bitflag_to_idx(op)];
	printf("Retrieved handler: 0x%x\n", handler);
	if (handler == NULL) {
		printf("recur for %s on operation %d (with parent: %s)\n", entry->name, bitflag_to_idx(op), entry->parent->name);
		return execute_handler( entry->parent, op, nargs, args);
	}

	res = handler(entry, args);
	va_end(args);

	return res;
}

void
create_entry(FSEntry *entry, int type, const char *path, handler_set handlers, FSEntry *parent, FSContent content) {
	entry->type = type;
	entry->id = 0;
	strcpy(entry->path, path);

	// get the path
	strcpy(entry->name, get_entry_name((char *) path));
	printf("name: %s\npath: %s\n\n", entry->name, entry->path);

	entry->handlers[bitflag_to_idx(GETATTR)] = handlers.getattr;
	entry->handlers[bitflag_to_idx(READLINK)] = handlers.readlink;
	entry->handlers[bitflag_to_idx(MKNOD)] = handlers.mknod;

	if (type == MOFS_DIR)
		entry->handlers[bitflag_to_idx(MKDIR)] = handlers.mkdir;

	entry->handlers[bitflag_to_idx(UNLINK)] = handlers.unlink;

	if (type == MOFS_DIR)
		entry->handlers[bitflag_to_idx(RMDIR)] = handlers.rmdir;

	entry->handlers[bitflag_to_idx(SYMLINK)] = handlers.symlink;
	entry->handlers[bitflag_to_idx(RENAME)] = handlers.rename;
	entry->handlers[bitflag_to_idx(LINK)] = handlers.link;
	entry->handlers[bitflag_to_idx(CHMOD)] = handlers.chmod;
	entry->handlers[bitflag_to_idx(CHOWN)] = handlers.chown;
	entry->handlers[bitflag_to_idx(TRUNCATE)] = handlers.truncate;
	entry->handlers[bitflag_to_idx(OPEN)] = handlers.open;

	if (type == MOFS_DIR)
		entry->handlers[bitflag_to_idx(READDIR)] = handlers.readdir;

	entry->handlers[bitflag_to_idx(READ)] = handlers.read;
  entry->handlers[bitflag_to_idx(UTIMENS)] = handlers.utimens;
	entry->handlers[bitflag_to_idx(CREATE)] = handlers.create;

  if (parent != NULL && parent->content.dir.num_children < MAX_CHILDREN) {
		printf("Setting entry %s parent to %s\n", entry->name, parent->name);
	  entry->parent = parent;
		printf("parent's num of children before: %d\n", parent->content.dir.num_children);
    parent->content.dir.children[parent->content.dir.num_children++] = entry;
		printf("parent's num of children after: %d\n", parent->content.dir.num_children);
  }

	entry->content = content;

	add_entry(*entry);
}

int
get_free_spot() {
	int i;

	for (i = 0; i < MAX_ENTRIES; i++) {
		if ((pool_bitmap[i >> 3] & 0x80 >> i % 8) == 0) {
			pool_bitmap[i >> 3] |= 0x80 >> i % 8;
			break;
		}
  }

	if (i == MAX_ENTRIES - 1) {
		fprintf(stderr, "no free space in FS!\n");
		return -1;
	}

	return i;
}
