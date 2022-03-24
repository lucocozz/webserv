#!/usr/local/bin/php
<html>
<head>
<title>Print the working directory.</title>
<body bgcolor="white">
<?php
$browser = getenv('HTTP_USER_AGENT');
echo "The users browser is $browser";
?>
</body>
</html>