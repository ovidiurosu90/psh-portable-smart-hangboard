<?php

define('PARENT_DIR', dirname(__DIR__));
require_once PARENT_DIR . '/vendor/autoload.php';
require_once PARENT_DIR . '/config.php';
require_once PARENT_DIR . '/app/Log.php';

class MyGoogleSheets
{
    const ACCESS_TOKEN_FILE = PARENT_DIR . '/token.json';
    const CREDENTIALS_FILE  = PARENT_DIR . '/credentials.json';

    /**
     * @return Google_Client the authorized client object
     */
    public function getClient()
    {
        $client = new Google_Client();
        putenv('GOOGLE_APPLICATION_CREDENTIALS=' . GOOGLE_APPLICATION_CREDENTIALS);
        $client->useApplicationDefaultCredentials();
        $client->addScope(\Google\Service\Sheets::SPREADSHEETS);

        return $client;
    }

    /**
     * @param bool $calibration
     * @return array<string: array<string, ...>, ...>
     */
    public function getConfig($calibration = false)
    {
        $spreadsheetId = PSH_SPREADSHEET_ID;
        $range = 'Config!A1:Z';
        if ($calibration) {
            $range = 'CalibrationConfig!A1:Z';
        }

        $client = $this->getClient();
        $service = new Google_Service_Sheets($client);

        $response = $service->spreadsheets_values->get($spreadsheetId, $range);
        $rows = $response->getValues();

        if (empty($rows) || !is_array($rows) || empty($rows[0])) {
            throw new Exception('Invalid Google Sheets Data');
        }

        $headers = array_shift($rows);
        $data = [];
        foreach ($rows as $row) {
            foreach ($row as $columnKey => $cell) {
                $data[$headers[$columnKey]][] = $cell;
            }
        }

        return $data;
    }

    /**
     * @param array<string: array<string, ...>, ...> $data
     * @return string
     */
    public function getConfigOutput($data)
    {
        $output = '';
        foreach ($data as $configKey => $configValues) {
            $output .= $configKey . '=' . implode(',', $configValues) . "\n";
        }
        return $output;
    }
}

