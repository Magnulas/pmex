def smooth(filtration, cocycle):
    from    cvxopt          import spmatrix, matrix
    from    cvxopt.blas     import copy
    from    lsqr            import lsqr

    coefficient = []
    coface_indices = []
    face_indices = []
    for i,s in enumerate(filtration):
        if s.dimension() > 2: continue

        c = 1
        for sb in s.boundary:
            j = filtration(sb)
            coefficient.append(c)
            coface_indices.append(i)
            face_indices.append(j)
            c *= -1

    # Cocycle can be larger than D; we implicitly project it down
    cocycle_max = max(zz[1] for zz in cocycle)

    # D is a coboundary matrix
    dimension = max(max(coface_indices), max(face_indices), cocycle_max) + 1
    D = spmatrix(coefficient, coface_indices, face_indices, (dimension, dimension))

    z = spmatrix([zz[0] for zz in cocycle],
                 [zz[1] for zz in cocycle],
                 [0     for zz in cocycle], (dimension, 1))

    v1 = D * z
    if bool(D*D):
        raise Exception('D^2 is not 0')
    if bool(v1):
        raise Exception('Expected a cocycle as input')
    z = matrix(z)

    def Dfun(x,y,trans = 'N'):
        if trans == 'N':
            copy(D * x, y)
        elif trans == 'T':
            copy(D.T * x, y)
        else:
            assert False, "Unexpected trans parameter"

    tol = 1e-10
    show = False
    maxit = None
    solution = lsqr(Dfun, matrix(z), show = show, atol = tol, btol = tol, itnlim = maxit)

    z_smooth = z - D*solution[0]

    # print sum(z_smooth**2)
    # assert sum((D*z_smooth)**2) < tol and sum((D.T*z_smooth)**2) < tol, "Expected a harmonic cocycle"
    if not (sum((D*z_smooth)**2) < tol and sum((D.T*z_smooth)**2) < tol):
        raise Exception("Expected a harmonic cocycle: %f %f" % (sum((D*z_smooth)**2), sum((D.T*z_smooth)**2)))

    values = []
    vertices = ((i,s) for (i,s) in enumerate(filtration) if s.dimension() == 0)
    for i,s in vertices:
        v = [v for v in s.vertices][0]
        if v >= len(values):
            values.extend((None for i in xrange(len(values), v+1)))
        values[v] = solution[0][i]

    return values
