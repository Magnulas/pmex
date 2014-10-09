:class:`ZigzagPersistence` class
================================

The class deals with the setting :math:`K_1 \rightarrow K_2 \leftarrow K_3 \rightarrow \dots`.
The :ref:`triangle-zigzag-example` illustrates the use of :class:`ZigzagPersistence`.

.. class:: ZigzagPersistence

    .. method:: add(boundary, birth)
        
        Adds a simplex with the given `boundary` to the complex, i.e. 
        :math:`K_{i+1} = K_i \cup \sigma` and `boundary` = :math:`\partial \sigma`.
        If a new class is born as a result of the addition, `birth` is stored with 
        it for future reference.

        :returns: a pair (`i`, `d`). The first element is the index `i`. 
                  It is the internal representation of the newly added simplex,
                  and should be used later for removal or when constructing the
                  boundaries of its cofaces. In other words, `boundary` must
                  consist of these indices.  The second element `d` is the death
                  element. It is `None` if a birth occurred, otherwise it
                  contains the value passed as `birth` to
                  :meth:`~ZigzagPersistence.add` or
                  :meth:`~ZigzagPersistence.remove` when the class that just
                  died was born.

    .. method:: remove(index, birth)
      
        Removes the simplex identified by the given `index` from the complex. If
        a new class is born as a result of the removal, `birth` is stored with
        it for future reference.
        
        :returns: `None` if a birth has occurred, otherwise it contains the value 
                  passed as `birth` to :meth:`~ZigzagPersistence.add` or
                  :meth:`~ZigzagPersistence.remove` when the class that just
                  died was born.

    .. method:: is_alive(z)

        Determines whether a given cycle is alive. The input should be an
        instance of :class:`ZNode`.

    .. method:: __iter__()

        Iterator over elements of type :class:`ZNode`, i.e. the cycles stored in the structure.

.. class:: ZNode

    .. attribute:: birth

        The birth value associated with the cycle. It is passed to
        :class:`ZigzagPersistence` in method
        :meth:`~ZigzagPersistence.add`.

    .. method:: __iter__()

        Iterator over the individual nodes (simplices) of the cycle (same
        indices that are passed to and returned by
        :meth:`~ZigzagPersistence.add`.


Auxilliary functions
--------------------

A pair of auxilliary functions is provided to help add and remove entire
collections of simplices. Both are pure Python functions defined in
:sfile:`bindings/python/dionysus/zigzag.py`.

    .. function:: add_simplices(zigzag, simplices, complex, birth, report_local = False)

        Adds each simplex in `simplices` to the `zigzag`. `complex` is a
        dictionary mapping simplices to their indices (in `zigzag`'s internal
        representation). All the newly born classes are given the value of
        `birth`.

        :returns: list of deaths that occur as a result of `simplices`' removal. 
                  Each death is a pair of the dimension of the class and the
                  `birth` value passed when the class was born.  By default the
                  deaths equal to `birth` are not reported unless `report_local`
                  is set to `True`.

    .. function:: remove_simplices(zigzag, simplices, complex, birth, report_local = False)

        Same parameters and return as in :func:`add_simplices` except that
        `simplices` are removed from the `zigzag` and the `complex`.



:class:`ImageZigzagPersistence` class
=====================================

The class deals with the setting 

.. math::
    \begin{array}{ccccccc}
        K_1         & \rightarrow   & K_2       &   \leftarrow      & K_3       & \rightarrow   & \dots \\
        \uparrow    &               & \uparrow  &                   & \uparrow  & \\
        L_1         & \rightarrow   & L_2       &   \leftarrow      & L_3       & \rightarrow   & \dots
    \end{array}

where the vertical maps are inclusions, i.e. :math:`L_i \subseteq K_i`.

.. class:: ImageZigzagPersistence

    .. method:: add(boundary, subcomplex, birth)
 
        Interface is the same as in :meth:`ZigzagPersistence.add`. The
        additional parameter `subcomplex` controls whether the simplex is added
        to :math:`L` or not. We always have :math:`K_{i+1} = K_i \cup \sigma`.
        If `subcomplex` is true, then :math:`L_{i+1} = L_i \cup \sigma`,
        otherwise :math:`L_{i+1} = L_i`.

    .. method:: remove(index, birth)
      
        Interface is exactly the same as in :meth:`ZigzagPersistence.remove`.        
