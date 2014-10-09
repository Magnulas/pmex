:class:`CohomologyPersistence` class
====================================

The :ref:`rips-pairwise-cohomology` illustrates the use of :class:`CohomologyPersistence`.

.. class:: CohomologyPersistence

    .. method:: __init__(prime = 11)

        Initializes :class:`CohomologyPersistence` with the given `prime`; from
        this point on all the computation will be performed with coefficients
        in :math:`\mathbb{Z}/prime \mathbb{Z}`.

    .. method:: add(boundary, birth, [store = True], [image = True], [coefficients = []])

        Adds a simplex with the given `boundary` to the complex, i.e.
        :math:`K_{i+1} = K_i \cup \sigma` and `boundary` = :math:`\partial \sigma`.
        If a new class is born as a result of the addition, `birth` is stored with
        it for future reference.

        If `store` is ``False`` and a class is born, it will not be stored in
        :class:`CohomologyPersistence`. This avoids wasting space on the
        classes of the dimension equal to the maximum-dimensional simplices of
        the complex since such classes will never die.

        The `image` parameter allows one to work with a case of a space
        :math:`L \subseteq K` where the filtration of :math:`K` induces a
        filtration of :math:`L`. In this case, one may want to compute **image
        persistence** (i.e. the persistence of the sequences of the images given
        by the inclusion of :math:`L` in :math:`K`). `image` indicates whether
        the simplex added belongs to :math:`L` or not.

        If given, `coefficients` is a list parallel to `boundary` that provides
        coefficients for the corresponding boundary elements. If empty, it is
        assumed to be :math:`(-1)^i`.

        :returns: a triple (`i`, `d`, `ccl`). The first element is the index `i`.
                  It is the internal representation of the newly added simplex,
                  and should be used later when constructing the
                  boundaries of its cofaces. In other words, `boundary` must
                  consist of these indices.  The second element `d` is the death
                  element. It is `None` if a birth occurred, otherwise it
                  contains the value passed as `birth` to
                  :meth:`~CohomologyPersistence.add` when the class that just
                  died was born.
                  The third element `ccl` returns the dying cocycle
                  (iterable over instances of :class:`CHSNode`), in case of a death.
                  It's empty if a birth occurs.

    .. method:: __iter__()

        Iterator over the live cocycles stored in
        :class:`CohomologyPersistence`. The returned elements are of the type
        :class:`Cocycle` below.


.. class:: Cocycle

    .. attribute:: birth

        The birth value associated with the cocycle. It is passed to
        :class:`CohomologyPersistence` in method
        :meth:`~CohomologyPersistence.add`.

    .. method:: __iter__()

        Iterator over the individual nodes (simplices) of the cocycle, each of type
        :class:`CHSNode`.

.. class:: CHSNode

    .. attribute:: si

        The index of the simplex, of type :class:`CHSimplexIndex`.

    .. attribute:: coefficient

        Coefficient in :math:`\mathbb{Z}/prime \mathbb{Z}` associated with the
        simplex.


.. class:: CHSimplexIndex

    .. attribute:: order

        The count associated with the simplex when it is inserted into
        :class:`CohomologyPersistence`.


Adaptor
-------

:class:`StaticCohomologyPersistence` provides a wrapper around
class :class:`CohomologyPersistence` that's compatible with :class:`StaticPersistence`.
See the documentation of the latter class for details.


.. class:: StaticCohomologyPersistence

   .. method:: __init__(filtration, prime = 2)

        Initializes :class:`StaticCohomologyPersistence` with the given
        :class:`Filtration`. `prime` is passed straight to the wrapped
        :class:`CohomologyPersistence` class.

   .. method:: pair_simplices()

        Pairs simplices using :class:`CohomologyPersistence` class.

   .. method:: __call__(i)

        Given a node in the internal representation, the method returns its
        integer offset from the beginning of the filtration.

   .. method:: make_simplex_map(filtration)

        Creates an auxilliary map from the nodes to the simplices::

            smap = persistence.make_simplex_map(filtration)
            for i in persistence:
                if i.unpaired(): print smap[i]

   .. method:: __iter__()

        Iterator over the nodes (representing individual simplices). See
        :class:`APNode`.

   .. method:: __len__()

        Returns the number of nodes (i.e. the number of simplices).

.. class:: APNode

    The following methods behave the same way as they do in :class:`SPNode`.

    .. method:: sign()

    .. method:: pair()

    .. method:: unpaired()

    The only crucial distinction in the behavior comes with the attribute
    :attr:`cocycle`.

    .. attribute:: cocycle

        If the simplex is positive, this attribute stores a cocycle it created (recorded at the time of its death).
        The 1-dimensional cocycles can be used with the :func:`circular.smooth` function to turn
        them into circle-valued maps.
        ::

            for i in persistence:
                if i.sign(): print i.cocycle


.. class:: ImagePersistence

    This class is another wrapper around :class:`CohomologyPersistence` that can
    compute image persistence induced by inclusion of a subcomplex. Its
    interface is the same as :class:`StaticCohomologyPersistence` above, except
    for the constructor:

    .. method:: __init__(filtration, subcomplex)

       `subcomplex` is a function called with every simplex. It should return
       ``True`` if the simplex belong to the subcomplex; ``False`` otherwise.


Circular coordinates
--------------------

.. function:: circular.smooth(filtration, cocycle)

   Returns a map from the vertices of the simplicial complex `filtration` to a circle :math:`[-.5, .5]`,
   where the opposite ends of the interval are identified.
