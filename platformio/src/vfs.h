#ifndef VFS_H
#define VFS_H
#include <stdio.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "rom/spi_flash.h"

void vfs_setup();

nvs_handle vfs_get_handle(const char *nspace);
void vfs_write_i32(nvs_handle handle, const char *key, int32_t value);
int32_t vfs_read_i32(nvs_handle handle, const char *key);

#endif /* VFS_H */
