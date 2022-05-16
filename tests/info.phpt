--TEST--
phpinfo()
--EXTENSIONS--
bzip3
--FILE--
<?php
phpinfo();
?>
--EXPECTF--
%a
bzip3

Bzip3 support => enabled
Extension Version => %d.%d.%d
Library Version => %s

%a
