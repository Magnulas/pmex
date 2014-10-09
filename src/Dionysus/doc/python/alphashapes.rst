.. _alphashapes:

Alpha shapes
============

There are two functions provided to compute alpha shapes. One in 2D and one in
3D. Both take a list of points (each a list of coordinates) as input, and fill a
list with the simplices of the `Delaunay triangulation`_. Each such simplex is
said to be *attached* (or *regular*) if its dual Voronoi cell does not contain a
critical point of the distance function to the point set. The smallest value of
the squared distance function on the dual Voronoi cell of the Delaunay simplex
is the alpha shape value assigned to it. This value is stored in the simplex's
`data` attribute; whether it is attached is stored in the `attached` attribute.

.. _`Delaunay triangulation`:   http://en.wikipedia.org/wiki/Delaunay_triangulation


.. function:: fill_alpha_complex(points, complex)

    Based on the dimension of the first point, decides which of the two functions
    below to call.

.. function:: fill_alpha2D_complex(points, complex)

    Appends to the `complex` the simplices of the 2D Delaunay triangulation
    on the `points`.

.. function:: fill_alpha3D_complex(points, complex)

    Appends to the `complex` the simplices of the 3D Delaunay triangulation
    on the `points`.


Example
-------

The following example generates 10 points on a circle, and computes their
Delaunay triangulation with corresponding alpha shape values::

    from math import sin, cos, pi
    points = [[cos(2*pi*t/10), sin(2*pi*t/10)] for t in xrange(10)]
    complex = Filtration()
    fill_alpha2D_complex(points, complex)

One can extract any given alpha shape with the usual Python list notation::

    alphashape = [s for s in complex if s.data[0] <= .5]
