#!/usr/bin/python
import sys, os, re

rootpath = sys.argv[1]
if not os.path.isdir(rootpath):
    print("Not valid folder")
    sys.exit(0)

linkfile = {}
regex = re.compile(r"-\s*\*\*\[.*\]\((.*)\)")
# Round 1
for root, dirs, files in os.walk(rootpath):
    for path in files:
        if re.search(".*.md$", path):
            print(f"process {path}")
            targetPath = os.path.join(root, path)
            with open(targetPath, 'r', encoding='utf-8') as file:
                for line_number, line in enumerate(file, start=1):
                    # Search for the pattern in the current line
                    match = regex.search(line)
                    if match:
                        # If a match is found, print the line and the line number
                        # print(f"{line.strip()}")
                        targetFile = match.group(1)
                        linkfile[os.path.basename(targetFile)] = path

# for key in linkfile:
#   print(f"{key} - {linkfile[key]}")

# Iterate for parent
processFileset = []
for key in linkfile:
    if linkfile[key] not in processFileset:
        processFileset.append(linkfile[key])
# print(processFileset)

# Process each parent
for root, dirs, files in os.walk(rootpath):
    # get level to be added
    for path in files:
        if path in processFileset:
            # Find the level
            curLevel = 1
            tempPath = path
            while tempPath in linkfile:
                tempPath = linkfile[tempPath]
                curLevel += 1
            targetPath = os.path.join(root, path)
            # Process
            with open(targetPath, 'r', encoding='utf-8') as file:
                modified_lines = []
                for line_number, line in enumerate(file, start=1):
                    match = regex.search(line)
                    if match:
                        # Process
                        processText = f"""
```{{include}} {match.group(1)}
:heading-offset: {curLevel}
```"""
                        modified_lines.append(processText)
                    else:
                        modified_lines.append(line)
            with open(targetPath, 'w', encoding='utf-8') as file:
                # Write all modified lines back to the file
                file.writelines(modified_lines)
