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

print "test script 1\n"
test = Test.new()
test.ptnMessage()
print "test script 2\n"
