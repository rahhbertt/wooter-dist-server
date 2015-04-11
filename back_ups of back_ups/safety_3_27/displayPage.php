#!/usr/bin/php

<?php

   $fp = stream_socket_client('localhost:13005', $errno, $errstr, 5);
   if (!$fp) {
       echo $errstr;
       exit(1);
   }
  fwrite($fp, "$argv[1]\r\n\r\n");

   while($line = fgets($fp)) {
       echo $line;
   }

   fclose($fp);
?>
