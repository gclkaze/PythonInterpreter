
def foo():
    print 'Foo'
    foo.cnt += 1
    for i in xrange(0,10000000*foo.cnt):
        pass
    return foo.cnt

foo.cnt = 1

def foo2(x,y):
    print 'Foo2'
    return x*y

def no_return():
    print "No return"
    return

def none_return():
    print "None return"
    return None

def dict_return():
    print "Dict return"
    return {'a':'dict','with':'2 keys'}

def string_return():
    print "String return"
    return "a string"
