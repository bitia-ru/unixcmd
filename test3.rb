$var1 = 'Test value'
@var2 = 'ooops'

def func2
    module X
        def func1
            p Module.nesting
        end
    end
end

class Foo
    def foo
        puts 'This is a class foo'
    end
end

foo = Foo.new

foo.foo

foo.extend X

foo.func1

