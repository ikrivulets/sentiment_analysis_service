#!/usr/bin/env bash

cp fastcgi-daemon /etc/nginx/sites-available/
ln -s /etc/nginx/sites-available/fastcgi-daemon /etc/nginx/sites-enabled/fastcgi-daemon

service nginx restart
