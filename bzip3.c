
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>
#include <zend_smart_str.h>
#include "php_bzip3.h"

/* bzip3 */
#include "libbz3.h"

#ifndef KiB
#define KiB(x) ((x)*1024)
#endif
#ifndef MiB
#define MiB(x) ((x)*1024 * 1024)
#endif

static int32_t read_int32(uint8_t * data)
{
  return ((uint32_t)data[0]) | (((uint32_t)data[1]) << 8)
    | (((uint32_t)data[2]) << 16) | (((uint32_t)data[3]) << 24);
}

static void write_int32(uint8_t * data, int32_t value)
{
  data[0] = value & 0xFF;
  data[1] = (value >> 8) & 0xFF;
  data[2] = (value >> 16) & 0xFF;
  data[3] = (value >> 24) & 0xFF;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_bzip3_compress, 0, 0, 1)
  ZEND_ARG_INFO(0, data)
  ZEND_ARG_INFO(0, block_size)
ZEND_END_ARG_INFO()
ZEND_FUNCTION(bzip3_compress)
{
  char *data;
  long block_size_mib = 8;
  uint8_t byteswap_buf[4], *encode_buf;
  uint32_t block_size;
  int32_t encode_size, read_size;
  size_t data_offset = 0, data_size;
  smart_str dest = {0};
  struct bz3_state *state;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l",
                            &data, &data_size, &block_size_mib) == FAILURE) {
    RETURN_THROWS();
  }

  block_size = MiB(block_size_mib);
  if (block_size < KiB(65) || block_size > MiB(511)) {
    zend_argument_value_error(2,
                              "block size must be between 65 KiB and 511 MiB");
    RETURN_THROWS();
  }

  smart_str_appendl(&dest, "BZ3v1", 5);

  write_int32(byteswap_buf, block_size);
  smart_str_appendl(&dest, byteswap_buf, 4);

  state = bz3_new(block_size);
  if (state == NULL) {
    zend_error(E_WARNING, "failed to create a block encoder state");
    RETURN_FALSE;
  }

  encode_buf = (uint8_t *)emalloc(block_size + block_size / 50 + 32);
  if (!encode_buf) {
    zend_error(E_WARNING, "failed to allocate memory");
    bz3_free(state);
    RETURN_FALSE;
  }

  while (data_size > 0) {
    if (data_size <= block_size) {
      memcpy(encode_buf, data + data_offset, data_size);
      read_size = data_size;
      data_size = 0;
    } else {
      memcpy(encode_buf, data + data_offset, block_size);
      read_size = block_size;
      data_offset += block_size;
      data_size -= block_size;
    }

    encode_size = bz3_encode_block(state, encode_buf, read_size);
    if (encode_size == -1) {
      zend_error(E_WARNING,
                 "failed to encode a block: %s", bz3_strerror(state));
      efree(encode_buf);
      bz3_free(state);
      smart_str_free(&dest);
      RETURN_FALSE;
    }

    write_int32(byteswap_buf, encode_size);
    smart_str_appendl(&dest, byteswap_buf, 4);
    write_int32(byteswap_buf, read_size);
    smart_str_appendl(&dest, byteswap_buf, 4);
    smart_str_appendl(&dest, encode_buf, encode_size);
  }

  efree(encode_buf);

  if (bz3_last_error(state) != BZ3_OK) {
    zend_error(E_WARNING, "failed to read data: %s", bz3_strerror(state));
    bz3_free(state);
    smart_str_free(&dest);
    RETURN_FALSE;
  }

  bz3_free(state);

  RETVAL_STR_COPY(dest.s);

  smart_str_free(&dest);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_bzip3_uncompress, 0, 0, 1)
  ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()
ZEND_FUNCTION(bzip3_uncompress)
{
  char *data;
  size_t data_offset = 0, data_size, buffer_size;
  uint8_t byteswap_buf[4], *decode_buf;
  uint32_t block_size;
  int32_t decode_size, read_size;
  smart_str dest = {0};
  struct bz3_state *state;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
                            &data, &data_size) == FAILURE) {
    RETURN_THROWS();
  }

  if (data_size < 5 || strncmp(data, "BZ3v1", 5) != 0) {
    zend_argument_value_error(1, "invalid signature");
    RETURN_THROWS();
  }
  data_offset += 5;

  if (data_offset + 4 > data_size) {
    zend_argument_value_error(1, "invalid block size in the header");
    RETURN_THROWS();
  }
  block_size = read_int32(data + data_offset);
  data_offset += 4;
  if (block_size < KiB(65) || block_size > MiB(511)) {
    zend_argument_value_error(1, "invalid block size in the header");
    RETURN_THROWS();
  }

  state = bz3_new(block_size);
  if (state == NULL) {
    zend_error(E_WARNING, "failed to create a block decoder state");
    RETURN_FALSE;
  }

  buffer_size = block_size + block_size / 50 + 32;
  decode_buf = (uint8_t *)emalloc(buffer_size);
  if (!decode_buf) {
    zend_error(E_WARNING, "failed to allocate memory");
    bz3_free(state);
    RETURN_FALSE;
  }

  while (data_offset < data_size) {
    if (data_offset + 4 > data_size) {
      break;
    }
    read_size = read_int32(data + data_offset);
    data_offset += 4;

    if (data_offset + 4 > data_size) {
      zend_error(E_WARNING, "invalid data error");
      bz3_free(state);
      efree(decode_buf);
      smart_str_free(&dest);
      RETURN_FALSE;

    }
    decode_size = read_int32(data + data_offset);
    data_offset += 4;

    if (data_offset + read_size > data_size) {
      zend_error(E_WARNING, "invalid data error");
      bz3_free(state);
      efree(decode_buf);
      smart_str_free(&dest);
      RETURN_FALSE;
    }
    memcpy(decode_buf, data + data_offset, read_size);
    data_offset += read_size;

    if (bz3_decode_block(state, decode_buf,
#ifdef HAVE_BZIP3_DECODE_BLOCK_NEW
                         buffer_size,
#endif
                         read_size, decode_size) == -1) {
      zend_error(E_WARNING,
                 "failed to decode a block: %s", bz3_strerror(state));
      bz3_free(state);
      efree(decode_buf);
      smart_str_free(&dest);
      RETURN_FALSE;
    }

    smart_str_appendl(&dest, decode_buf, decode_size);
  }

  efree(decode_buf);

  if (bz3_last_error(state) != BZ3_OK) {
    zend_error(E_WARNING, "failed to read data: %s", bz3_strerror(state));
    bz3_free(state);
    smart_str_free(&dest);
    RETURN_FALSE;
  }

  bz3_free(state);

  if (dest.s) {
    RETVAL_STR_COPY(dest.s);
  } else {
    RETVAL_EMPTY_STRING();
  }

  smart_str_free(&dest);
}

ZEND_MINFO_FUNCTION(bzip3)
{
  php_info_print_table_start();
  php_info_print_table_row(2, "Bzip3 support", "enabled");
  php_info_print_table_row(2, "Extension Version", PHP_BZIP3_EXT_VERSION);
  php_info_print_table_row(2, "Library Version", LIBBZIP3_VERSION);
  php_info_print_table_end();
}

static zend_function_entry bzip3_functions[] = {
  ZEND_NS_FALIAS(PHP_BZIP3_NS, compress,
                 bzip3_compress, arginfo_bzip3_compress)
  ZEND_NS_FALIAS(PHP_BZIP3_NS, uncompress,
                 bzip3_uncompress, arginfo_bzip3_uncompress)
  {NULL, NULL, NULL}
};

zend_module_entry bzip3_module_entry = {
  STANDARD_MODULE_HEADER,
  "bzip3",
  bzip3_functions,
  NULL,
  NULL,
  NULL,
  NULL,
  ZEND_MINFO(bzip3),
  PHP_BZIP3_EXT_VERSION,
  STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_BZIP3
ZEND_GET_MODULE(bzip3)
#endif
