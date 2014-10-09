:class:`StaticPersistence` class
================================

.. class:: StaticPersistence

    .. method:: __init__(filtration)

        Initializes :class:`StaticPersistence` with the given
        :class:`Filtration`. This operation effectively computes the boundary
        matrix of the complex captured by the filtration with rows and columns
        sorted with respect to the filtration ordering.

    .. method:: pair_simplices(store_negative = False)

        Pairs simplices using the [ELZ02]_ algorithm. `store_negative` indicates
        whether to store the negative simplices in the cycles.

    .. method:: __call__(i)

        Given an SPNode in the internal representation, the method returns its
        integer offset from the beginning of the filtration. This is useful to
        lookup the actual name of the simplex in the complex.

    .. method:: make_simplex_map(filtration)

        Creates an auxilliary :class:`PersistenceSimplexMap` used to lookup the actual
        simplices from the persistence indices. For example, the following
        snippet prints out all the unpaired simplices::

            smap = persistence.make_simplex_map(filtration)
            for i in persistence:
                if i.unpaired(): print smap[i]

    .. method:: __iter__()

        Iterator over the nodes (representing individual simplices). See
        :class:`SPNode`.

    .. method:: __len__()

        Returns the number of nodes (i.e. the number of simplices).


.. class:: SPNode

    The class represents nodes stored in :class:`StaticPersistence`. These nodes
    are aware of their :meth:`sign` and :attr:`pair` (and :meth:`cycle` if
    negative after :meth:`StaticPersistence.pair_simplices` has run).

    .. method:: sign()

        Returns the sign of the simplex: `True` for positive, `False` for
        negative.

    .. method:: pair()

        Simplex's pair. The pair is set to self if the siplex is unpaired.

    .. attribute:: cycle

        If the simplex is negative, its cycle (that it kills) is non-empty, and
        can be accessed using this method. The cycle itself is an iterable
        container of :class:`SPNode`. For example, one can print the basis for
        the (bounding) cycles::

            smap = persistence.make_simplex_map(filtration)
            for i in persistence:
                for ii in i.cycle: print smap[ii]

    .. method:: unpaired()

        Indicates whether the simplex is unpaired.

.. class:: SPersistenceSimplexMap

    .. method:: __getitem__(i)

        Given a persistence index, i.e. an :class:`SPNode`, returns the
        :class:`Simplex` it represents.


:class:`DynamicPersistenceChains` class
=======================================

.. class:: DynamicPersistenceChains

    This class works exactly like :class:`StaticPersistence`, providing all the
    same methods. The only difference is that when iterating over it, the
    elements are of type :class:`DPCNode`, described below. 

.. class:: DPCNode

    This class works just like :class:`SPNode`, except it has an additional
    attribute :attr:`chain`. 

    .. attribute:: chain
    
        It allows one to retrieve the "chain" associated with the simplex. 
        (In terms of the :math:`R = DV` decomposition, it gives access to the
        columns of the matrix :math:`V`.) In case of the positive simplex, this
        is a cycle created by the addition of this simplex.  This access is
        particularly useful for the unpaired positive simplices, allowing one to
        recover the cycles they create. In case of the negative simplex, this chain's
        boundary is exactly what's stored in the :attr:`~SPNode.cycle` attribute.
    
        For example, to print out all the essential cycles of the complex, one
        can run the following loop::

            smap = persistence.make_simplex_map(filtration)
            for i in persistence:
                if i.unpaired()
                    for ii in i.chain: print smap[ii]

