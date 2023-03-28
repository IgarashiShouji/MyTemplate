MRuby::Gem::Specification.new('mruby-mystub') do |spec|
  spec.license = 'MIT'
  spec.authors = ['My Utilities', 'Shouji, Igarashi']
  spec.summary = 'cross complie target stub class'

#  spec.linker.libraries << "mystub"
#  spec.linker.library_paths 'Objects'
  #if spec.for_windows?
  #  spec.linker.libraries << "ws2_32"
  #end
  #spec.add_test_dependency 'mruby-time', core: 'mruby-time'
end
