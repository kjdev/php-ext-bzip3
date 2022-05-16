--TEST--
Bzip3\compress()/Bzip3\uncompress()
--EXTENSIONS--
bzip3
--FILE--
<?php
$string = "Life it seems, will fade away
Drifting further everyday
Getting lost within myself
Nothing matters no one else";

var_dump(Bzip3\compress(1));

var_dump(Bzip3\uncompress(Bzip3\compress(1)));

$data = Bzip3\compress($string);
var_dump(Bzip3\uncompress($data));

echo "Done\n";
?>
--EXPECTF--
string(%d) "BZ3v1%a"
string(1) "1"
string(110) "Life it seems, will fade away
Drifting further everyday
Getting lost within myself
Nothing matters no one else"
Done
