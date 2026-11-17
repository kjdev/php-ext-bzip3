--TEST--
Bzip3\uncompress() rejects a header block size that exceeds memory_limit
--EXTENSIONS--
bzip3
--INI--
memory_limit=64M
--FILE--
<?php
var_dump(Bzip3\uncompress("BZ3v1" . pack('V', 511 * 1024 * 1024)));

echo "Done\n";
--EXPECTF--
Warning: block size in the header exceeds the memory limit in %s on line %d
bool(false)
Done
