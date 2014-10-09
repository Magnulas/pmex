:class:`Simplex` class
======================

.. class:: Simplex

    .. method:: __init__(seq[, data])
    
        Initializes :class:`Simplex` from the given sequence `seq` and
        optionally real value `data`, e.g.::
    
            s = Simplex([1,2,3], 7.8)

    .. method:: add(v)
        
        Adds vertex `v` to the simplex, increasing its dimension by 1.

        .. seealso:: :meth:`~Simplex.join`

    .. attribute:: boundary

        Iterator over the boundary :math:`\partial \sigma` of the simplex,
        e.g.::
            
            for sb in s.boundary: print sb

    .. method:: contains(v)

        :returns: `True` iff the simplex contains vertex `v`.

    .. method:: dimension()

        :returns: the dimension of the simplex (one less than its number of
                  vertices).

    .. method:: join(other)
        
        Joins the current simplex with the `other` simplex. The method copies over 
        the vertices from the `other` simplex.

    .. attribute:: data
        
        Real value stored in the simplex.

    .. attribute:: vertices

        (Sorted) vertices of the simplex accessible as a sequence, e.g.::
        
            for v in s.vertices: print v,

    .. method:: __hash__()
    .. method:: __eq__(other)

        Simplices are hashable, and equality comparable, and therefore can be
        stored in a dictionary.  Simplices are equal if their
        :attr:`~Simplex.vertices` are the same.


Utility functions for manipulating simplices
--------------------------------------------

The first function :func:`vertex_cmp` is a Python interface to a C++ function.
The rest are pure Python functions defined in
:sfile:`bindings/python/dionysus/__init__.py`.

.. function:: vertex_cmp(s1, s2)
    
    Compares the two simplices with respect to the lexicographic order of their vertices.

.. function:: vertex_dim_cmp(s1, s2)
    
    Compares the two simplices with respect to their dimension, and lexicographically 
    within the same dimension.

.. function:: data_cmp(s1, s2)
    
    Compares the two simplices with respect to the data (real values) they
    store.

.. function:: data_dim_cmp(s1, s2)
    
    Compares the two simplices with respect to their dimension and within the same 
    dimension with respect to their data.


