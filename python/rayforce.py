#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from ctypes import *
import sys

class GUID(Structure):
    _fields_ = [("buf", c_ubyte * 16)]

class Obj(Union):
    _fields_ = [
        ("bool", c_bool),
        ("u8", c_ubyte),
        ("vchar", c_char),
        ("i64", c_int64),
        ("f64", c_double),
        # ... add other fields
    ]

class Obj_t(Structure):
    _fields_ = [
        ("mmod", c_ubyte),
        ("refc", c_ubyte),
        ("type", c_ubyte),
        ("attrs", c_ubyte),
        ("rc", c_uint32),
        ("data", Obj)  # Use the union here
    ]

raylib = CDLL('../librayforce.so')
raylib.i64.restype = POINTER(Obj_t)
raylib.i64.argtypes = [c_int64]

runtime_init = raylib.runtime_init
runtime_init.argtypes = [c_int32, POINTER(c_char_p)]
runtime_init.restype = None

def i64(val):
    obj_ptr = raylib.i64(val)
    return obj_ptr.contents

def use_obj_t(obj):
    obj_ptr = pointer(obj)
    raylib.some_c_function(obj_ptr)


# Convert Python list of arguments to array of strings
argv = (c_char_p * len(sys.argv))(*map(bytes, sys.argv, ['utf-8'] * len(sys.argv)))
argc = c_int32(len(sys.argv))

# Call runtime_init
runtime_init(argc, argv)
