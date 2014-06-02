#!/usr/bin/ruby

require 'continuation'

class Exception
  class NoContinuation < StandardError
  end
  
  attr_accessor :continuation
  
  def continue
    raise NoContinuation unless continuation.respond_to?(:call)
    continuation.call
  end
end

module NeverGonnaLetYouDown
  def raise(exception = RuntimeError, string = nil, array = caller)
    # With a single String argument, raises a
    # RuntimeError with the string as a message. 
    if exception.is_a?(String)
      string = exception
      exception = RuntimeError
    end
    
    callcc do |cc|
      obj = exception.class == Class ? exception.exception(string) : exception
      obj.set_backtrace(array)
      obj.continuation = cc
      super obj
    end
  end
  
  def fail(exception = RuntimeError, string = nil, array = caller)
    raise(exception, string, array)
  end
end

class Object
  include NeverGonnaLetYouDown
end

#10.times do |i|
#  begin
#    raise "OH NO!"
#    puts "OH YES! #{i}"
#    i += 1
#  rescue => err
#    if i < 5
#      # Nah, let's ignore this exception and just continue
#      # from where it was raised.
#      err.continue
#    else
#      raise err
#    end
#  end
#end


