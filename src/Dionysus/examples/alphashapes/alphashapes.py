# Computes the persistence diagram of the alpha shapes in both 2D and 3D 
# (decided dynamically based on the input file)

from    dionysus        import Filtration, StaticPersistence, data_dim_cmp, vertex_cmp, \
                               fill_alpha3D_complex, fill_alpha2D_complex, points_file
from    sys             import argv, exit
from    math            import sqrt


if len(argv) < 2:
    print "Usage: %s POINTS" % argv[0]
    exit()

points = [p for p in points_file(argv[1])]
f = Filtration()
if   len(points[0]) == 2:           # 2D
    fill_alpha2D_complex(points, f)
elif len(points[1]) == 3:           # 3D
    fill_alpha3D_complex(points, f)

print "Total number of simplices:", len(f)

f.sort(data_dim_cmp)
print "Filtration initialized"

p = StaticPersistence(f)
print "StaticPersistence initialized" 

p.pair_simplices()
print "Simplices paired"

print "Outputting persistence diagram"
smap = p.make_simplex_map(f)
for i in p:
    if i.sign():
        b = smap[i]
        if i.unpaired():
            print b.dimension(), sqrt(b.data[0]), "inf"
            continue

        d = smap[i.pair()]
        print b.dimension(), sqrt(b.data[0]), sqrt(d.data[0])
