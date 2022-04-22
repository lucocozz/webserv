#!/usr/local/bin/php
<html>
<head>
<title>Print the working directory.</title>
<body bgcolor="white">
<?php
$browser = getenv('HTTP_USER_AGENT');
$scriptName = getenv('SCRIPT_NAME');
$serverName = getenv('SERVER_NAME');
echo "The users browser is $browser \n";
echo "The script's name is $scriptName \n";
echo "The server's name $serverName";

?>

</body>
</html>

