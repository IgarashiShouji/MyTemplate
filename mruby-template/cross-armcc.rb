# arm cortex-m3
#   compiler: kail arm compiler
MRuby::CrossBuild.new("armcc") do |conf|
  toolchain :armcc

  conf.cc do |cc|
    cc.command          = "armcc.exe"
    cc.flags            = "--c99 --gnu --cpu Cortex-M4.fp -g -O2 -Ospace --gnu --apcs=interwork --split_sections --exceptions --enum_is_int"
    cc.compile_options  = %Q[%{flags} -o "%{outfile}" -c "%{infile}"]
    cc.defines << %w(MRB_USE_FLOAT32)
    cc.defines << %w(MRB_HEAP_PAGE_SIZE=64)
    cc.defines << %w(KHASH_DEFAULT_SIZE=8)
    cc.defines << %w(MRB_GC_STRESS)
    cc.defines << %w(MRB_NO_STDIO) #if you dont need stdio.
  end

  conf.cxx do |cxx|
    cxx.command         = conf.cc.command.dup
    cxx.include_paths   = conf.cc.include_paths.dup
    cxx.flags           = conf.cc.flags.dup
    cxx.defines         = conf.cc.defines.dup
    cxx.compile_options = conf.cc.compile_options.dup
  end

  conf.linker do |linker|
    linker.command  ="armlink.exe"
  end

  conf.archiver do |archiver|
    archiver.command          = "armar.exe"
    archiver.archive_options  = '-rcs "%{outfile}" %{objs}'
  end

  #no executables
  conf.bins = []

  #do not build executable test
  conf.build_mrbtest_lib_only

  #disable C++ exception
  conf.disable_cxx_exception

  #gems from core
  conf.gem :core => "mruby-array-ext"       # Use Array class extension
  conf.gem :core => "mruby-class-ext"       # Use class/module extension
  conf.gem :core => "mruby-compar-ext"      # Use Comparable module extension
  conf.gem :core => "mruby-complex"         # Use Complex numbers
  conf.gem :core => "mruby-enum-ext"        # Use Enumerable module extension
  conf.gem :core => "mruby-enum-lazy"       # Use Enumerator::Lazy class (require mruby-enumerator)
  conf.gem :core => "mruby-enumerator"      # Use Enumerator class (require mruby-fiber)
  conf.gem :core => "mruby-eval"            # Use eval()
  conf.gem :core => "mruby-fiber"           # Use Fiber class
  conf.gem :core => "mruby-hash-ext"        # Use Hash class extension
  conf.gem :core => "mruby-kernel-ext"      # Use Kernel module extension
  conf.gem :core => "mruby-math"            # Use standard Math module
  conf.gem :core => "mruby-metaprog"        # Meta-programming features
  conf.gem :core => "mruby-method"          # Use Method/UnboundMethod class
  conf.gem :core => "mruby-numeric-ext"     # Use Numeric class extension
  conf.gem :core => "mruby-object-ext"      # Use Object class extension
  conf.gem :core => "mruby-objectspace"     # Use ObjectSpace class
  conf.gem :core => "mruby-proc-ext"        # Use Proc class extension
  conf.gem :core => "mruby-range-ext"       # Use Range class extension
  conf.gem :core => "mruby-rational"        # Use Rational numbers
  conf.gem :core => "mruby-sprintf"         # Use standard Kernel#sprintf method
  conf.gem :core => "mruby-string-ext"      # Use String class extension
  conf.gem :core => "mruby-struct"          # Use standard Struct class
  conf.gem :core => "mruby-symbol-ext"      # Use Symbol class extension
  conf.gem :core => "mruby-toplevel-ext"    # Use toplevel object (main) methods extension

  #light-weight regular expression
  #conf.gem :github => "masamitsu-murase/mruby-hs-regexp", :branch => "master"

  #Arduino API
  #conf.gem :github =>"kyab/mruby-arduino", :branch => "master"

  conf.disable_presym
end
