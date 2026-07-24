#!/usr/bin/env python3
"""Jolt Physics module config.py for GnuChan Engine SCons build."""

def can_build(env, platform):
    return True

def configure(env):
    pass

def get_doc_classes():
    return [
        "JoltPhysicsServer",
    ]

def get_doc_path():
    return "doc_classes"
