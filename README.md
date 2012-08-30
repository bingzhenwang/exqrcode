# Exqrcode
A php extension generating qrcode ,using libqrencode

## install

### Requirements
Linux only
php 5.3
libqrencode
(you can see the  details of libqrencode on https://github.com/fukuchi/libqrencode)
libpng

 this extension has been  developmenting now on ubuntu,  so the following tips may be helpful for you

1.apt-get install libpng12-0 libpng12-dev

2.
wget http://fukuchi.org/works/qrencode/qrencode-3.3.1.tar.gz
tar -zxvf qrencode-3.3.1.tar.gz
cd qrencode-3.3.1
./configure make && make && install

3.download exqrcode && unzip exqrcode zip file

./configure --with-qrencode --with-png

4.add extension=exqrcode.so into php.ini
retart apache 

5.test php

## base usage

$instance = new ExQrcode();
$instance->setLevel(ExQrcode::QRLEVEL_H);
$instance->setSize(7);

$instance->setDpi(72);

#encode general text

$instance->encodeGeneral("hello world");
var_dump($instance->writeToFile("/tmp/qrgeneral.png"));

#encode mecard

$arr = array("N" => "name","TEL" => "13900000000","EMAIL" => "email" 
, "ADDR" => "address" ,"URL" => "http://www.helloworld.com");

$instance->encodeMECard($arr);
var_dump($instance->writeToFile("/tmp/qrmecard.png"));

#encode url

$instance->encodeUrl("http://www.baidu.com");
var_dump($instance->writeToFile("/tmp/qrurl.png"));

#encode phone

$instance->encodePhone("13900000000");
var_dump($instance->writeToFile("/tmp/qrphone.png"));

#encode sms

$instance->encodeSMS("message subject","message content");
var_dump($instance->writeToFile("/tmp/qrsms.png"));




## class and interface

class name : ExQrcode
	setDpi($dpinumber) default is 72
	setSize($size) the dot pixels ,default is 3
	setLevel($level) specify error correction level, ExQrcode::QRLEVEL_L,ExQrcode::QRLEVEL_M,ExQrcode::QRLEVEL_Q,ExQrcode::QRLEVEL_H,represent lowest to highest 	
	encodeGeneral($text) encode general text
	encodeUrl($url) encode a url 
	encodeMECard($arr) encode a me card ,parameter is an array,key 'N' means name, 'TEL' means a telphone,'EMAIL' means an email, 'ADDR' menas an address,'URL' means a http url
	encodePhone($phonenumber) encode a phonenumber
	encodeSMS($smssubject,$content) encode a sms item
	encodeICAL(arr) todo
