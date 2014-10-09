from dionysus import Simplex, Filtration, DynamicPersistenceChains, \
                     vertex_cmp, data_cmp, data_dim_cmp \

complex = [Simplex((0,),        0),                 # A
           Simplex((1,),        1),                 # B
           Simplex((2,),        2),                 # C
           Simplex((0,1),       2.5),               # AB
           Simplex((1,2),       2.9),               # BC
           Simplex((0,2),       3.5)]               # CA

print "Complex:", complex
print "Vertex: ", sorted(complex, vertex_cmp)
print "Data:   ", sorted(complex, data_cmp)
print "DataDim:", sorted(complex, data_dim_cmp)

f = Filtration(complex, data_cmp)
print "Complex in the filtration order:", ', '.join((str(s) for s in f))

p = DynamicPersistenceChains(f)
print "Persistence initialized"
p.pair_simplices()
print "Simplices paired"

smap = p.make_simplex_map(f)
for i in p:
    print i.sign(), i.pair().sign()
    print "%s (%d) - %s (%d)" % (smap[i], i.sign(), smap[i.pair()], i.pair().sign())
    print "Cycle (%d):" % len(i.cycle), " + ".join((str(smap[ii]) for ii in i.cycle))
    
    if i.unpaired():
        print "Chain (%d):" % len(i.chain), " + ".join((str(smap[ii]) for ii in i.chain))

print "Number of unpaired simplices:", len([i for i in p if i.unpaired()])
