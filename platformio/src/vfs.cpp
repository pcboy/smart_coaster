
#include "vfs.h"

void vfs_setup() {
  nvs_flash_init();
}

nvs_handle vfs_get_handle(const char *nspace) {
  nvs_handle my_handle;
  esp_err_t err;

  err = nvs_open(nspace, NVS_READWRITE, &my_handle);
  if (err != ESP_OK) {
    fprintf(stderr, "Error (%d) opening NVS!\n", err);
    return((nvs_handle)NULL);
  }

  return(my_handle);
}
void vfs_write_i32(nvs_handle handle, const char *key, int32_t value) {
  nvs_set_i32(handle, key, value);
  nvs_commit(handle);
}

int32_t vfs_read_i32(nvs_handle handle, const char *key) {
  int32_t value;

  nvs_get_i32(handle, key, &value);
  return(value);
}
