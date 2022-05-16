--TEST--
Bzip3\compress(): heredoc
--EXTENSIONS--
bzip3
--FILE--
<?php
$expected = <<<HEREDOC
This is some random data
HEREDOC;

$actual = Bzip3\uncompress(Bzip3\compress($expected));

var_dump($expected === $actual);
?>
--EXPECTF--
bool(true)
