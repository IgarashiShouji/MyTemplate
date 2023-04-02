#! /bin/bash --norc

if [ ! -d mruby ]; then
  git clone https://github.com/mruby/mruby.git
  cd mruby
  git checkout 3.1.0 -b rev-3.1.0
  #git checkout 3.2.0 -b rev-3.2.0
fi
