import os
import re

SEARCH = "// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008"

# Aranacak kelimeler
WORDS = []
for w in re.findall(r"[A-Za-z0-9@._]+", SEARCH):
    if len(w) > 1:  # C gibi tek harfleri at
        WORDS.append(re.compile(rf"\b{re.escape(w)}\b"))

ROOT = os.path.dirname(os.path.abspath(__file__))

for root, _, files in os.walk(ROOT):
    for file in files:
        path = os.path.join(root, file)

        try:
            with open(path, "r", encoding="utf-8", errors="ignore") as f:
                for line_no, line in enumerate(f, 1):
                    matched = [p.pattern[2:-2] for p in WORDS if p.search(line)]
                    if matched:
                        print(f"{path}:{line_no}")
                        print(f"Eşleşen: {', '.join(matched)}")
                        print(line.rstrip())
                        print()
        except Exception:
            pass