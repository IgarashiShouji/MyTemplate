#
#   test script
#

class Test
  def initialize
    @obj = MyStub.new
  end
  def ptnMessage
    @obj.test()
    @obj.test2()
  end
end
