<?php

require_once dirname(__DIR__) . '/app/MyGoogleSheets.php';
$myGoogleSheets = new MyGoogleSheets();

try {
    echo $myGoogleSheets->postResults($_POST);
} catch (Exception $e) {
    Log::error('Exception caught while posting results: ' . $e->getMessage());
    header($_SERVER['SERVER_PROTOCOL'] . ' 500 Internal Server Error!', true, 500);
    exit(1);
}

