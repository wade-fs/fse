#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys

try:
    import yaml
except ImportError:
    print("❌ Error: PyYAML package is not installed.")
    print("Please install it running: pip install pyyaml")
    sys.exit(1)

def lint_yaml_directory(directory):
    success = True
    total_files = 0
    failed_files = 0

    print(f"🔍 Scanning directory: {directory}")
    for root, _, files in os.walk(directory):
        for f in files:
            if f.endswith('.yaml') or f.endswith('.yml'):
                total_files += 1
                path = os.path.join(root, f)
                try:
                    with open(path, 'r', encoding='utf-8') as fh:
                        yaml.safe_load(fh.read())
                except Exception as e:
                    success = False
                    failed_files += 1
                    print(f"❌ Syntax Error in {path}:")
                    print(f"   {e}")
                    print("-" * 50)
    
    if success:
        print(f"✨ All {total_files} YAML configurations parsed successfully!")
    else:
        print(f"⚠️ Lint Failed: {failed_files} out of {total_files} files contain errors.")
    return success

if __name__ == "__main__":
    target_dir = "mudlib/content"
    if len(sys.argv) > 1:
        target_dir = sys.argv[1]
    
    if not os.path.exists(target_dir):
        print(f"❌ Error: Target directory '{target_dir}' does not exist.")
        sys.exit(1)

    ok = lint_yaml_directory(target_dir)
    sys.exit(0 if ok else 1)
