--TEST--
Bzip3\uncompress(): invalid data
--EXTENSIONS--
bzip3
--FILE--
<?php
$string = "Life it seems, will fade away
Drifting further everyday
Getting lost within myself
Nothing matters no one else";

// invalid signature
foreach ([1, $string] as $data) {
  try {
    var_dump(Bzip3\uncompress($data));
  } catch (Error $e) {
    echo $e->getMessage(), PHP_EOL;
  }
}

// invalid block size in the header
foreach (["BZ3v1", "BZ3v10000"] as $data) {
  try {
    var_dump(Bzip3\uncompress($data));
  } catch (Error $e) {
    echo $e->getMessage(), PHP_EOL;
  }
}

// empty
$data = Bzip3\compress($string);
var_dump(Bzip3\uncompress(substr($data, 0, 9)));

// invalid data error
var_dump(Bzip3\uncompress(substr($data, 0, 13)));
var_dump(Bzip3\uncompress(substr($data, 0, -1)));

// failed to decode a block
var_dump(Bzip3\uncompress(substr($data, 0, -2) . "xx"));

// OK
var_dump(Bzip3\uncompress($data));
echo "Done\n";
?>
--EXPECTF--
Bzip3\uncompress(): Argument #1 ($data) invalid signature
Bzip3\uncompress(): Argument #1 ($data) invalid signature
Bzip3\uncompress(): Argument #1 ($data) invalid block size in the header
Bzip3\uncompress(): Argument #1 ($data) invalid block size in the header
string(0) ""

Warning: invalid data error in %s on line %d
bool(false)

Warning: invalid data error in %s on line %d
bool(false)

Warning: failed to decode a block: CRC32 check failed in %s on line %d
bool(false)
string(110) "Life it seems, will fade away
Drifting further everyday
Getting lost within myself
Nothing matters no one else"
Done
