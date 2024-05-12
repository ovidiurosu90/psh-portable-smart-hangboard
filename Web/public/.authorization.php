<?php

require_once dirname(__DIR__) . '/config.php';
require_once dirname(__DIR__) . '/app/Log.php';

if (!function_exists('getallheaders')) {
    function getallheaders()
    {
        $headers = [];
        foreach ($_SERVER as $name => $value) {
            if (substr($name, 0, 5) == 'HTTP_') {
                $headers[str_replace(' ', '-', ucwords(strtolower(str_replace(
                    '_', ' ', substr($name, 5)))))] = $value;
            }
        }
        return $headers;
    }
}

$headers = getallheaders();
if (empty($headers['Pshtoken']) || $headers['Pshtoken'] != PSH_TOKEN ) {
    Log::notice('Unauthorized attempt', [
        'SCRIPT_FILENAME' => basename($_SERVER['SCRIPT_FILENAME'])
    ]);
    header("HTTP/1.1 401 Unauthorized");
    exit;
}

