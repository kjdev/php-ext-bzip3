--TEST--
Bzip3\uncompress() rejects block sizes that exceed the buffer or are negative
--EXTENSIONS--
bzip3
--FILE--
<?php
$header = "BZ3v1" . pack('V', 66560);

// read_size larger than the buffer sized from the header
$readSize = 300000;
var_dump(Bzip3\uncompress(
    $header . pack('V', $readSize) . pack('V', 16) . str_repeat("A", $readSize)
));

// negative read_size
var_dump(Bzip3\uncompress(
    $header . pack('l', -1) . pack('V', 16) . str_repeat("A", 64)
));

// negative decode_size
var_dump(Bzip3\uncompress(
    $header . pack('V', 64) . pack('l', -1) . str_repeat("A", 64)
));

// decode_size larger than the buffer
var_dump(Bzip3\uncompress(
    $header . pack('V', 64) . pack('V', 300000) . str_repeat("A", 64)
));

echo "Done\n";
?>
--EXPECTF--
Warning: invalid data error in %s on line %d
bool(false)

Warning: invalid data error in %s on line %d
bool(false)

Warning: invalid data error in %s on line %d
bool(false)

Warning: invalid data error in %s on line %d
bool(false)
Done
