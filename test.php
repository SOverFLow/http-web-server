<?php

$target_dir = "../uploads/";
$nofile = false;
$target_file = "";
$imageFileType = "";
if (isset($_FILES["fileToUpload"]))
{
	$target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
	$uploadOk = 1;
	$imageFileType = strtolower(pathinfo($target_file, PATHINFO_EXTENSION));
}
else {
	$nofile = true;
	$uploadOk = 0;
}

echo "<!DOCTYPE html>", "\n";
echo "<html>", "\n", "\n";
echo "<head>", "\n";

echo "</head>", "\n", "\n";

echo '</header>', "\n";
echo '<body class="main">', "\n";

if ($nofile)
	echo "Please, choose a file.<br>", "\n";
else if (file_exists($target_file))
{
	echo "Sorry, file already exists.<br>", "\n";
	$uploadOk = 0;
}

if ($uploadOk == 0)
	echo "Your file was not uploaded.<br>", "\n";
else
{
	if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file))
		echo "The file ". htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])). " has been uploaded.<br>", "\n";
	else
		echo "Sorry, there was an error uploading your file.<br>", "\n";
}

echo '</body>', "\n";
echo "</html>", "\n";

?>