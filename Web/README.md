# Web
A simple API written in PHP to communicate with Google Sheets, where we store the configuration and the results of the training.

## Configure web server
```bash
cd /var/www/[DOMAIN]/public_html/
ln -s [REPOSITORY]/Web/public psh
```

## Downloading composer.phar (check [wiki](https://getcomposer.org/download/))
```bash
cd [REPOSITORY]/Web/
php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');"
php -r "if (hash_file('sha384', 'composer-setup.php') === 'dac665fdc30fdd8ec78b38b9800061b4150413ff2e3b6f88543c636f7cd84f6db9189d43a81e5503cda447da73c7e5b6') { echo 'Installer verified'; } else { echo 'Installer corrupt'; unlink('composer-setup.php'); } echo PHP_EOL;"
php composer-setup.php
php -r "unlink('composer-setup.php');"
```

## Install & configure PHP 8.3 on Ubuntu
```bash
sudo apt-get update
sudo add-apt-repository ppa:ondrej/php
sudo apt-cache search php | grep 8
sudo apt-get install php8.3 php8.3-cli php8.3-curl php8.3-xml php8.3-fpm
php -v
```

### Running PHP 8.3 along other PHP versions
Check source [here](https://www.phoca.cz/blog/1302-running-multiple-php-instances-with-lamp-stack-on-ubuntu-kubuntu-linux)
```bash
sudo vim /etc/php/8.3/fpm/pool.d/www.conf # add your own user & group there
sudo a2enmod actions fcgid alias proxy_fcgi rewrite

vim [REPOSITORY]/Web/public/.htaccess
#############
<FilesMatch \.php>
    SetHandler "proxy:unix:/var/run/php/php8.3-fpm.sock|fcgi://localhost/"
</FilesMatch>
#############

sudo systemctl restart apache2
```

## Installing dependencies
```bash
php composer.phar install
```

## Setup Google Sheets
- Create service account following the [wiki](https://github.com/googleapis/google-api-php-client/blob/main/docs/oauth-server.md#creating-a-service-account), and download the json file with the keys
- Enable Google Sheets API by going to the Google Sheets API overview in Google Cloud ([here](https://console.cloud.google.com/apis/library/sheets.googleapis.com))
- Create a new spreadsheet, and share it with the email address from the json file form above (field client_email)
- Create 3 sheets (Config, CalibrationConfig, Results)
1. Config

| totalCycles | activityTotalSeconds | restTotalSeconds | scaleLowerLimitGrams | scaleUpperLimitGrams |
| ----------- | -------------------- | ---------------- | -------------------- | -------------------- |
| 4           | 10                   | 20               | 50000                | 75000                |

2. CalibrationConfig

| knownWeightGrams |
| ---------------- |
| 87200            |

3. Results

| When                       | Score | SessionUID | History                                                     |
| -------------------------- | ----- | ---------- | ----------------------------------------------------------- |
| Tue, May 14 at 09:24:24 AM | 278   | 12941      | 11,15,14,13,17,14,13,12,19,12,15,14,14,15,14,13,13,13,15,12 |

Please note the custom date and time format. You'll have to set the same in your spreadsheet

## Configure the API
```bash
cp config.php.example config.php; vim config.php
cp credentials.json.example credentials.json; vim credentials.json
cp public/.user.ini.example public/.user.ini
```

## Testing the API
- Download [Postman](https://www.postman.com/downloads/)
- Add PSHTOKEN header (it needs to match with the value in config.php)
- Make a request to https://[DOMAIN]/psh/psh_get_config.php

