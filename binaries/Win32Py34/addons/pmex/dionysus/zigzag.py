def add_simplices(zz, simplices, complex, birth, report_local = False):
    deaths = []
    for s in simplices:
        i,d = zz.add([complex[sb] for sb in s.boundary], (s.dimension(), birth))
        complex[s] = i
        if d is not None:
            if report_local or not d[1] == birth:
                deaths.append(d)
    return deaths
            

def remove_simplices(zz, simplices, complex, birth, report_local = False):
    deaths = []
    for s in simplices:
        d = zz.remove(complex[s], (s.dimension() - 1, birth))
        complex[s] = None
        if d is not None:
            if report_local or not d[1] == birth:
                deaths.append(d)
    return deaths
