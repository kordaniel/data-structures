# Segment Tree class
Provides operations to update and get the values/sums of every segment in the array. Running time is O(Log n) and space complexity O(n), since the size of the array is always the smallest power of 2, that is at least the size of 2n. More formally, the smallest x, where 2^x <= 2n is true.


### Example run
```
SegmentTree size: 16
(1): {0, 0}
├── (2): {0, 0}
|   ├── (4): {0, 0}
|   |   ├── (8): {0, 0}
|   |   |   ├── (16): {0, 0}
|   |   |   └── (17): {0, 0}
|   |   └── (9): {0, 0}
|   |       ├── (18): {0, 0}
|   |       └── (19): {0, 0}
|   └── (5): {0, 0}
|       ├── (10): {0, 0}
|       |   ├── (20): {0, 0}
|       |   └── (21): {0, 0}
|       └── (11): {0, 0}
|           ├── (22): {0, 0}
|           └── (23): {0, 0}
└── (3): {0, 0}
    ├── (6): {0, 0}
    |   ├── (12): {0, 0}
    |   |   ├── (24): {0, 0}
    |   |   └── (25): {0, 0}
    |   └── (13): {0, 0}
    |       ├── (26): {0, 0}
    |       └── (27): {0, 0}
    └── (7): {0, 0}
        ├── (14): {0, 0}
        |   ├── (28): {0, 0}
        |   └── (29): {0, 0}
        └── (15): {0, 0}
            ├── (30): {0, 0}
            └── (31): {0, 0}
(1): {9, 0}
├── (2): {9, 0}
|   ├── (4): {9, 0}
|   |   ├── (8): {3, 0}
|   |   |   ├── (16): {0, 0}
|   |   |   └── (17): {0, 3}
|   |   └── (9): {0, 3}
|   |       ├── (18): {0, 0}
|   |       └── (19): {0, 0}
|   └── (5): {0, 0}
|       ├── (10): {0, 0}
|       |   ├── (20): {0, 0}
|       |   └── (21): {0, 0}
|       └── (11): {0, 0}
|           ├── (22): {0, 0}
|           └── (23): {0, 0}
└── (3): {0, 0}
    ├── (6): {0, 0}
    |   ├── (12): {0, 0}
    |   |   ├── (24): {0, 0}
    |   |   └── (25): {0, 0}
    |   └── (13): {0, 0}
    |       ├── (26): {0, 0}
    |       └── (27): {0, 0}
    └── (7): {0, 0}
        ├── (14): {0, 0}
        |   ├── (28): {0, 0}
        |   └── (29): {0, 0}
        └── (15): {0, 0}
            ├── (30): {0, 0}
            └── (31): {0, 0}
SUM of segment 1-3: 9
------------------------------
(1): {27, 0}
├── (2): {21, 0}
|   ├── (4): {13, 0}
|   |   ├── (8): {3, 0}
|   |   |   ├── (16): {0, 0}
|   |   |   └── (17): {3, 0}
|   |   └── (9): {0, 5}
|   |       ├── (18): {0, 0}
|   |       └── (19): {0, 0}
|   └── (5): {0, 2}
|       ├── (10): {0, 0}
|       |   ├── (20): {0, 0}
|       |   └── (21): {0, 0}
|       └── (11): {0, 0}
|           ├── (22): {0, 0}
|           └── (23): {0, 0}
└── (3): {6, 0}
    ├── (6): {6, 0}
    |   ├── (12): {0, 2}
    |   |   ├── (24): {0, 0}
    |   |   └── (25): {0, 0}
    |   └── (13): {2, 0}
    |       ├── (26): {0, 2}
    |       └── (27): {0, 0}
    └── (7): {0, 0}
        ├── (14): {0, 0}
        |   ├── (28): {0, 0}
        |   └── (29): {0, 0}
        └── (15): {0, 0}
            ├── (30): {0, 0}
            └── (31): {0, 0}
SUM of segment 1-10: 27
SUM of segment 2-10: 24
SUM of segment 2-3:  10
------------------------------
(1): {-18, 0}
├── (2): {-9, 0}
|   ├── (4): {3, 0}
|   |   ├── (8): {3, 0}
|   |   |   ├── (16): {0, 0}
|   |   |   └── (17): {3, 0}
|   |   └── (9): {0, 0}
|   |       ├── (18): {0, 0}
|   |       └── (19): {0, 0}
|   └── (5): {0, -3}
|       ├── (10): {0, 0}
|       |   ├── (20): {0, 0}
|       |   └── (21): {0, 0}
|       └── (11): {0, 0}
|           ├── (22): {0, 0}
|           └── (23): {0, 0}
└── (3): {-9, 0}
    ├── (6): {-9, 0}
    |   ├── (12): {0, -3}
    |   |   ├── (24): {0, 0}
    |   |   └── (25): {0, 0}
    |   └── (13): {-3, 0}
    |       ├── (26): {2, -5}
    |       └── (27): {0, 0}
    └── (7): {0, 0}
        ├── (14): {0, 0}
        |   ├── (28): {0, 0}
        |   └── (29): {0, 0}
        └── (15): {0, 0}
            ├── (30): {0, 0}
            └── (31): {0, 0}
SUM of segment 1-1:  3
SUM of segment 2-3:  0
SUM of segment 4-10: -21
SUM of segment 1-10: -18
------------------------------
(1): {-15, 0}
├── (2): {-6, 0}
|   ├── (4): {6, 0}
|   |   ├── (8): {3, 1}
|   |   |   ├── (16): {0, 0}
|   |   |   └── (17): {3, 0}
|   |   └── (9): {1, 0}
|   |       ├── (18): {0, 1}
|   |       └── (19): {0, 0}
|   └── (5): {0, -3}
|       ├── (10): {0, 0}
|       |   ├── (20): {0, 0}
|       |   └── (21): {0, 0}
|       └── (11): {0, 0}
|           ├── (22): {0, 0}
|           └── (23): {0, 0}
└── (3): {-9, 0}
    ├── (6): {-9, 0}
    |   ├── (12): {0, -3}
    |   |   ├── (24): {0, 0}
    |   |   └── (25): {0, 0}
    |   └── (13): {-3, 0}
    |       ├── (26): {-3, 0}
    |       └── (27): {0, 0}
    └── (7): {0, 0}
        ├── (14): {0, 0}
        |   ├── (28): {0, 0}
        |   └── (29): {0, 0}
        └── (15): {0, 0}
            ├── (30): {0, 0}
            └── (31): {0, 0}
SUM of segment 0-0: 1
SUM of segment 0-1: 5
SUM of segment 0-2: 6
Total SUM in array: -15
------------------------------
```
