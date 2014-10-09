:class:`Filtration` class
=========================

.. class:: Filtration
    
    This class serves as a representation of the simplicial complex. It knows both 
    how to perform a fast lookup of a given simplex, as well as how to 
    iterate over the simplices in a sorted order.

    .. method:: __init__()
    .. method:: __init__(simplices, cmp)
    
        Initializes :class:`Filtration` by internally storing the elements of the sequence
        `simplices`, and  in the order sorted with respect to `cmp`.

    .. method:: append(s)
        
        Appends the given simplex `s` to the filtration.

    .. method:: sort(cmp)

        Sorts the filtration with respect to the comparison `cmp`.

    .. method:: __getitem__(i)

        Random access to the elements of the filtration.

    .. method:: __call__(s)
        
        Finds the integer index of the given simplex in the sorted order of the filtration.

    .. method:: __iter__()
 
        Iterator over the elements of the filtration sorted with respect
        to the comparison `cmp`. E.g.::

            simplices = [Simplex([0], 2), ..., Simplex([3,4,5], 3.5)]
            f = Filtration(simplices, data_dim_cmp)
            for s in f: print s

    .. method:: __len__()

        Size of the filtration.
