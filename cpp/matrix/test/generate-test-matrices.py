#!/usr/bin/env python3

import os
import numpy as np

DATAPATH = './data'

if not os.path.exists(DATAPATH):
    print(f'Creating output directory `{DATAPATH}`')
    os.makedirs(DATAPATH)
else:
    print(f'Using output directory: `{DATAPATH}`')

DATATYPE = np.single # 32 bit float
GENERATED_RAND_SIZES = set()


def int_width(n: int) -> int:
    return int(np.log10(n)) + 1


def num_to_str(num: int, width: int) -> str:
    return ''.join(('0' * (width-int_width(num)), str(num)))


def generate_testcase_square(size: int, max_size: int = 100):
    if size > max_size:
        raise ValueError('Size is larger than (default?) max_size')
    else:
        num_width = int_width(max_size)

    GENERATED_RAND_SIZES.add((size, size))

    A = np.random.rand(size, size).astype(DATATYPE)
    B = np.random.rand(size, size).astype(DATATYPE)
    C = np.matrix(A) * np.matrix(B)

    np.savetxt(
        f'{DATAPATH}/square_{num_to_str(size, num_width)}',
        np.r_[A,B,C],
        header=f'{size}x{size}'
    )


def generate_testcase_random(max_size = 100):
    num_width = int_width(max_size)
    len1 = np.random.randint(1, max_size + 1)
    len2 = np.random.randint(1, max_size + 1)

    while (len1, len2) in GENERATED_RAND_SIZES:
        len1 = np.random.randint(1, max_size)
        len2 = np.random.randint(1, max_size)

    GENERATED_RAND_SIZES.add((len1, len2))

    fpath = f'{DATAPATH}/rand_{num_to_str(len1, num_width)}x{num_to_str(len2, num_width)}'

    A = np.random.rand(len1, len2).astype(DATATYPE)
    B = np.random.rand(len2, len1).astype(DATATYPE)
    C = np.matrix(A) * np.matrix(B)

    np.savetxt(
        f'{fpath}-A',
        A,
        header=f'{len1}x{len2}'
    )
    np.savetxt(
        f'{fpath}-B',
        B,
        header=f'{len2}x{len1}'
    )
    np.savetxt(
        f'{fpath}-C',
        C,
        header=f'{len1}x{len1}'
    )


MAX_SIZE = 50 # max length of matrix in one dimension

print(f'Generating {2*MAX_SIZE} different test cases with a maximum size of {MAX_SIZE} in both dimensions')
for size in range(1, MAX_SIZE+1):
    if size % 10 == 0:
        print(f'{2*size} test cases generated and saved in {DATAPATH}/')

    # Generate 2 square matrices of sizexsize and their product
    generate_testcase_square(size, MAX_SIZE)

    # Generate 2 matrices of random size < MAX_SIZE for both dimensions and their product
    generate_testcase_random(MAX_SIZE)

print('Done')
