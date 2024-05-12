<?php

require_once dirname(__DIR__) . '/app/MyGoogleSheets.php';
$myGoogleSheets = new MyGoogleSheets();

try {
    if (!empty($_GET['calibration'])) {
        echo $myGoogleSheets->getConfigOutput($myGoogleSheets->getConfig(true));
    } else {
        echo $myGoogleSheets->getConfigOutput($myGoogleSheets->getConfig());
    }
} catch (Exception $e) {
    Log::error('Exception caught while getting config: ' . $e->getMessage());
    header($_SERVER['SERVER_PROTOCOL'] . ' 500 Internal Server Error!', true, 500);
    exit(1);
}

