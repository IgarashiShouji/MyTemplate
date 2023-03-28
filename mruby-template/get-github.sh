#! /bin/bash --norc

if [ ! -d mruby ]; then
  git clone https://github.com/mruby/mruby.git
#  ln -s mruby-mystub mruby/mrbgems/
  cd mruby
  git checkout 3.2.0 -b rev-3.2.0
fi
