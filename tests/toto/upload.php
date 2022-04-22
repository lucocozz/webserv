<?php
$uploaddir = '/tmp';
$uploadfile = $uploaddir . basename($_FILES['userfile']['name']);

echo '<pre>';
if (move_uploaded_file($_FILES['userfile']['tmp_name'], $uploadfile)) {
    echo "The file has been uploaded\n";
} else {
    echo "Error during the file upload\n";
}

echo 'Debug infos:';
print_r($_FILES);

echo '</pre>';

?>

