#!/bin/sh

sigint()
{
   ps | grep test | awk '{print $1}' | xargs kill -9
   exit 0
}

trap 'sigint' INT

./test $1 &
node main.js

