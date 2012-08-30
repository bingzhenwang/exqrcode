<?php
$br = (php_sapi_name() == "cli")? "":"<br>";


$instance = new ExQrcode();
//ExQrcode::QRLEVEL_L
//ExQrcode::QRLEVEL_M
//ExQrcode::QRLEVEL_Q
//ExQrcode::QRLEVEL_H
$instance->setLevel(ExQrcode::QRLEVEL_H);
$instance->setSize(7);
//$instance->setDpi(72);
$instance->encodeGeneral("hello world");
var_dump($instance->writeToFile("/tmp/qrgeneral.png"));
$instance->encodeUrl("http://www.baidu.com");
var_dump($instance->writeToFile("/tmp/qrurl.png"));
$arr = array("N" => "name","TEL" => "13900000000","EMAIL" => "email" 
, "ADDR" => "address" ,"URL" => "http://www.helloworld.com");

$instance->encodeMECard($arr);
var_dump($instance->writeToFile("/tmp/qrmecard.png"));

$instance->encodePhone("13900000000");
var_dump($instance->writeToFile("/tmp/qrphone.png"));

$instance->encodeSMS("message subject","message content");
var_dump($instance->writeToFile("/tmp/qrsms.png"));
//var_dump($instance);

//
?>
