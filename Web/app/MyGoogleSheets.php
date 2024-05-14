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

    /**
     * @param array<string: string> $postData
     * @return int (1 if newly added, 0 if already existed)
     */
    public function postResults($postData)
    {
        $spreadsheetId = PSH_SPREADSHEET_ID;
        $range = 'Results!A1:Z';

        $client = $this->getClient();
        $service = new Google_Service_Sheets($client);

        $response = $service->spreadsheets_values->get($spreadsheetId, $range);
        $rows = $response->getValues();

        if (empty($rows) || !is_array($rows) || empty($rows[0])) {
            throw new Exception('Invalid Google Sheets Data');
        }

        $headers = array_shift($rows);
        $storedData = [];
        if (!empty($rows)) {
            foreach ($rows as $row) {
                foreach ($row as $columnKey => $cell) {
                    $storedData[$headers[$columnKey]][] = $cell;
                }
            }
        } else {
            foreach ($headers as $header) {
                $storedData[$header] = [];
            }
        }

        if (!isset($storedData['When']) || !isset($storedData['SessionUID'])) {
            throw new Exception('Invalid Google Sheets Data. ' .
                'Columns When & SessionUID are mandatory');
        }

        $currentDateTime = new DateTime('now');
        $dateTimeFormat = 'D, M d \a\t h:i:s A';
        foreach ($storedData['When'] as $whenKey => $whenValue) {
            $storedDateTime = DateTime::createFromFormat($dateTimeFormat, $whenValue);
            if ($storedDateTime &&
                $currentDateTime->format('Y-m-d') == $storedDateTime->format('Y-m-d') &&
                $storedData['SessionUID'][$whenKey] == $postData['sessionUID']
            ) {
                Log::info('Results already posted: ' . print_r($postData, true));
                return 0;
            }
        }

        $valueRange = new Google_Service_Sheets_ValueRange();
        $valueRange->setValues([
            [
                (new DateTime('now'))->format($dateTimeFormat),
                $postData['scoreValue'],
                $postData['sessionUID'],
                $postData['scoreHistory'],
            ]
        ]);
        $range = 'Results'; // the service will detect the last row of this sheet
        $options = ['valueInputOption' => 'USER_ENTERED'];
        $service->spreadsheets_values->append($spreadsheetId, $range, $valueRange,
            $options);
        Log::info('Results posted: ' . print_r($postData, true));
        return 1;
    }
}

