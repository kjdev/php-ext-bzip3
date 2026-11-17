--TEST--
Bzip3\compress(): block_size
--EXTENSIONS--
bzip3
--INI--
memory_limit=528M
--FILE--
<?php
$string = "Life it seems, will fade away
Drifting further everyday
Getting lost within myself
Nothing matters no one else";

foreach ([0, 1, 100, 200, 511, 512] as $block) {
  echo "=== block size: {$block}", PHP_EOL;
  try {
    $data = Bzip3\compress($string, $block);
    if ($block > 1) {
      // uncompress() needs about 7x the block size of memory_limit
      var_dump(substr($data, 0, 5) . bin2hex(substr($data, 5, 4)));
      continue;
    }
    var_dump(Bzip3\uncompress($data));
  } catch (Error $e) {
    echo $e->getMessage(), PHP_EOL;
  }
}

echo "Done\n";
?>
--EXPECTF--
=== block size: 0
Bzip3\compress(): Argument #2 ($block_size) block size must be between 65 KiB and 511 MiB
=== block size: 1
string(110) "Life it seems, will fade away
Drifting further everyday
Getting lost within myself
Nothing matters no one else"
=== block size: 100
string(13) "BZ3v100004006"
=== block size: 200
string(13) "BZ3v10000800c"
=== block size: 511
string(13) "BZ3v10000f01f"
=== block size: 512
Bzip3\compress(): Argument #2 ($block_size) block size must be between 65 KiB and 511 MiB
Done
