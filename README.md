# Bzip3 Extension for PHP

This extension allows [bzip3](https://github.com/kspalaiologos/bzip3).

## Build

``` sh
% git clone --recursive --depth=1 https://github.com/kjdev/php-ext-bzip3.git
% cd php-ext-bzip3
% phpize
% ./configure
% make
$ make install
```

To use the system library.

``` sh
% ./configure --with-libbzip3
```

## Configuration

php.ini:

``` sh
extension=bzip3.so
```

## Usage

``` php
$compress = Bzip3\compress($data);
$var = Bzip3\uncompress($compress);
```

## Function synopsis

``` php
Bzip3\comporess(string $data, int $block_size = 8): string|false
Bzip3\uncomporess(string $data): string|false
```

---

### Bzip3\compress

``` php
Bzip3\compress(string $data, int $block_size = 8): string|false
```

This function compress a string.

**Parameters:**

* data

  The data to compress.

* block_size

  compress block size in MiB. (Defaults to 8)

**Return Values:**

The compressed string or FALSE if an error occurred.

---

### Bzip3\uncompress

``` php
Bzip3\uncompress(string $data): string|false
```

This function uncompress a compressed string.

**Parameters:**

* data

  The data compressed by Bzip3\compress().

**Return Values:**

The original uncompressed data or FALSE on error.
