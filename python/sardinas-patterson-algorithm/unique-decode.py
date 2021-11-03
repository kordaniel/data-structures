'''
Python3 implementation of the Sardinas-Patterson algorithm to check whether a
variable-length code is uniquely decodable.

A code is uniquely decodable if every string of symbols constructed from its
code-words can be uniquely decomposed into exactly one sequence of code-words.

https://en.wikipedia.org/wiki/Sardinas–Patterson_algorithm
https://towardsdatascience.com/the-sardinas-patterson-algorithm-in-simple-python-9718242752c3
https://github.com/danhales/blog-sardinas-patterson
'''

def generate_cn(c, n):
    if n == 0:
        return set(c)

    cn = set()
    cn_minus_1 = generate_cn(c, n-1)

    for u in c:
        for v in cn_minus_1:
            if (len(u) > len(v)) and u.find(v) == 0:
                cn.add(u[len(v):])

    for u in cn_minus_1:
        for v in c:
            if (len(u) > len(v)) and u.find(v) == 0:
                cn.add(u[len(v):])

    return cn

def generate_c_infinity(c):
    cs = []
    c_infinity = set()
    n = 1
    cn = generate_cn(c, n)
    #print('c_{}'.format(n), cn)

    while len(cn) > 0:
        if cn in cs:
            #print('Cycle detected. Halting algorithm.')
            break

        cs.append(cn)
        c_infinity = c_infinity.union(cn)
        n += 1
        cn = generate_cn(c, n)
        #print('c_{}'.format(n), c_infinity)

    return c_infinity

def sardinas_patterson_theorem(c):
    '''
    Returns true if c is uniquely decodable
    '''
    c_infinity = generate_c_infinity(c)
    return len(c & c_infinity) == 0 # Intersection of the 2 sets

def check_decodability(c):
    if sardinas_patterson_theorem(c):
        print(c, 'is uniquely decodable', sep='\n')
    else:
        print(c, 'is not uniquely decodable', sep='\n')


if __name__ == '__main__':
    # Codes
    # Let      a ,  b,    c,    d, and so on
    #          =    =     =     =
    c1 = set(['0', '11', '001', '101']) # Uniquely decodable.
    c2 = set(['0', '11', '010', '101']) # This is not uniquely decodable.
                                        # For example: adda = 0.101.101.0 = 010.11.010 = cbc.
    c3 = set(['10', '110', '00', '111', '011', '010']) # Uniquely decodable.
    codes = (c1, c2, c3)

    for c in codes:
        check_decodability(c)
        print()
