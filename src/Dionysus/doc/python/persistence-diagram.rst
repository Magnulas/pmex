:class:`PersistenceDiagram` class
==================================

.. class:: PersistenceDiagram

    .. method:: __init__( dimension )

        Initializes : an empty( no points ) :class:`PersistenceDiagram` object and sets
        the :attr:`~PersistenceDiagram.dimension` attribute( must be integer ) e.g.::

            dia = PersistenceDiagram( 1 )

    .. method:: __init__( dimension, point_seq )

        Initializes :class:`PersistenceDiagram` of specified dimension from the given sequence `seq` of tuples, e.g.::

            dia = PersistenceDiagram( 1, (1,2) )

        The tuples must have at least 2 elements ``(birth, death)``.
        If there is a third element, it is stored as extra data associated to
        a point.

    .. method:: append( p )

        Adds point `p` to the persistence diagram.

    .. attribute:: dimension

        Dimension of the persistence diagram. Must be an integer. Must be set at initialization.

    .. method:: __iter__( )

        Iterator over the points in the persistence diagram,
        e.g.::

            for p in dia: print p

    .. method:: __len__( )

        :returns: The number of points in the diagram.



Utility functions for persistence diagrams
--------------------------------------------

.. function:: init_diagrams(persistence, filtration[, eval = lambda s: s.data[, data = lambda i: None]])

    Initializes a collection of :class:`PersistenceDiagram` instances from `persistence`
    and `filtration`. Optional `eval` can determine how to extract birth and
    death values from a simplex. For example, if `filtration` was filled using
    :func:`fill_alpha_complex()`, the :attr:`~Simplex.data` contains a pair ``(value, critical)``.
    We can extract the ``value`` from the tuple::

        init_diagrams(persistence, filtration, lambda s: s.data[0])

    Optional `data` argument can return arbitrary data to associate with each point,
    given an node of `persistence`.

.. function:: bottleneck_distance(dia1, dia2)

    Calculates the bottleneck distance between the two persistence diagrams.

.. function:: wasserstein_distance(dia1, dia2, p)

    Calculates the `p`-th Wasserstein distance between the two persistence diagrams.

