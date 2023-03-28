#! /bin/bash

cd mruby
export MRUBY_CONFIG=../default.rb
#ruby minirake --trace $1
ruby minirake $1
