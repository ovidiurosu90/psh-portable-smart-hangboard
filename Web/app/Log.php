<?php

require_once dirname(__DIR__) . '/vendor/autoload.php';

use Monolog\Logger;
use Monolog\Handler\RotatingFileHandler;

class Log
{
    protected static $instance;

    private function __construct()
    {
    }

    /**
     * Method to return the Monolog instance
     *
     * @return \Monolog\Logger
     */
    static public function getLogger()
    {
        if (!self::$instance) {
            self::configureInstance();
        }

        return self::$instance;
    }

    /**
     * Configure Monolog to use a rotating files system.
     */
    protected static function configureInstance()
    {
        $dir = dirname(__DIR__) . '/logs';

        if (!file_exists($dir)){
            mkdir($dir, 0774, true);
            chgrp($dir, 'www-data');
        }

        $logger = new Logger('my_logger');
        $logger->pushHandler(new RotatingFileHandler($dir . '/' . php_sapi_name() . '.log', 31));
        self::$instance = $logger;
    }

    public static function debug($message, array $context = [])
    {
        self::getLogger()->debug($message, $context);
    }

    public static function info($message, array $context = [])
    {
        self::getLogger()->info($message, $context);
    }

    public static function notice($message, array $context = [])
    {
        self::getLogger()->notice($message, $context);
    }

    public static function warning($message, array $context = [])
    {
        self::getLogger()->warning($message, $context);
    }

    public static function error($message, array $context = [])
    {
        self::getLogger()->error($message, $context);
    }

    public static function critical($message, array $context = [])
    {
        self::getLogger()->critical($message, $context);
    }

    public static function alert($message, array $context = [])
    {
        self::getLogger()->alert($message, $context);
    }

    public static function emergency($message, array $context = [])
    {
        self::getLogger()->emergency($message, $context);
    }
}

// Log::info("something really interesting happened", ['extra' => 'information', 'about' => 'anything' ]);

