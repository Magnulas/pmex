from dionysus import Simplex, ZigzagPersistence, \
                     vertex_cmp, data_cmp \
#                    ,enable_log

complex = {Simplex((0,),        0):     None,                   # A
           Simplex((1,),        1):     None,                   # B
           Simplex((2,),        2):     None,                   # C
           Simplex((0,1),       2.5):   None,                   # AB
           Simplex((1,2),       2.9):   None,                   # BC
           Simplex((0,2),       3.5):   None,                   # CA
           Simplex((0,1,2),     5):     None}                   # ABC

print "Complex:"
for s in sorted(complex.keys()): print s
print

#enable_log("topology/persistence")
zz = ZigzagPersistence()

# Add all the simplices
b = 1
for s in sorted(complex.keys(), data_cmp):
    print "%d: Adding %s" % (b, s)
    i,d = zz.add([complex[ss] for ss in s.boundary], b)
    complex[s] = i
    if d:   print "Interval (%d, %d)" % (d, b-1)
    b += 1

# Remove all the simplices
for s in sorted(complex.keys(), data_cmp, reverse = True):
    print "%d: Removing %s" % (b, s)
    d = zz.remove(complex[s], b)
    del complex[s]
    if d:   print "Interval (%d, %d)" % (d, b-1)
    b += 1
