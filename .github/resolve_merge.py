#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause
""" Resolve the mechanical conflicts of a merge from the rtems.org repository. """

# Copyright (C) 2026 embedded brains GmbH & Co. KG
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# The eb repository builds a qualified subset of RTEMS.  It therefore deletes
# large parts of the build specification which the rtems.org repository keeps
# extending.  Every extension inside a deleted region conflicts as a
# delete-versus-modify hunk whose 'ours' side is empty.
#
# Such a hunk is resolved in favour of 'ours'.  This encodes the policy that a
# new source of the rtems.org repository stays outside the qualification scope
# until it is added deliberately.  Silently inheriting it would change what
# gets certified.
#
# Every other hunk is left untouched.  A hunk with content on both sides is a
# qualification scope decision and must be made by a human.

import argparse
import subprocess
import sys

_BEGIN = "<<<<<<< "

_BASE = "||||||| "

_SEPARATOR = "======="

_END = ">>>>>>> "


def _is(line: str, marker: str) -> bool:
    return line.startswith(marker)


def _is_separator(line: str) -> bool:
    return line.rstrip("\r\n") == _SEPARATOR


def _parse(lines: list[str]) -> list[tuple]:
    """ Splits the lines into text blocks and conflict hunks. """
    blocks: list[tuple] = []
    text: list[str] = []
    index = 0
    count = len(lines)
    while index < count:
        if not _is(lines[index], _BEGIN):
            text.append(lines[index])
            index += 1
            continue
        if text:
            blocks.append(("text", text))
            text = []
        begin = index
        index += 1
        ours: list[str] = []
        while index < count and not _is(lines[index],
                                       _BASE) and not _is_separator(
                                           lines[index]):
            ours.append(lines[index])
            index += 1
        # The diff3 and zdiff3 conflict styles add the merge base.
        if index < count and _is(lines[index], _BASE):
            index += 1
            while index < count and not _is_separator(lines[index]):
                index += 1
        theirs: list[str] = []
        if index < count and _is_separator(lines[index]):
            index += 1
            while index < count and not _is(lines[index], _END):
                theirs.append(lines[index])
                index += 1
        if index >= count:
            raise RuntimeError("unterminated conflict hunk")
        index += 1
        blocks.append(("conflict", ours, theirs, lines[begin:index]))
    if text:
        blocks.append(("text", text))
    return blocks


def _resolve(path: str) -> tuple[str, int, int]:
    """ Returns the resolved content and the hunk counts of the file. """
    with open(path, "r", encoding="utf-8", errors="surrogateescape") as src:
        lines = src.read().splitlines(keepends=True)
    out: list[str] = []
    resolved = 0
    remaining = 0
    for block in _parse(lines):
        if block[0] == "text":
            out.extend(block[1])
            continue
        _, ours, _theirs, raw = block
        if ours:
            remaining += 1
            out.extend(raw)
        else:
            resolved += 1
    return "".join(out), resolved, remaining


def _conflicted_files() -> list[str]:
    stdout = subprocess.check_output(
        ["git", "diff", "--name-only", "--diff-filter=U"], encoding="utf-8")
    return [line for line in stdout.splitlines() if line.strip()]


def main(argv: list[str]) -> int:
    """ Resolves the mechanical conflicts of the in-progress merge. """
    parser = argparse.ArgumentParser(description=sys.modules[__name__].__doc__)
    parser.add_argument("--dry-run",
                        action="store_true",
                        help="do not write the files and do not stage them")
    args = parser.parse_args(argv[1:])
    files = _conflicted_files()
    if not files:
        print("There are no conflicted files.")
        return 0
    report: list[str] = []
    blocked = False
    for path in files:
        content, resolved, remaining = _resolve(path)
        if remaining:
            blocked = True
            report.append(
                f"* `{path}`: {resolved} declined, **{remaining} need a "
                "qualification scope decision**")
            continue
        report.append(
            f"* `{path}`: {resolved} declined, file unchanged")
        if args.dry_run:
            continue
        with open(path, "w", encoding="utf-8",
                  errors="surrogateescape") as dst:
            dst.write(content)
        subprocess.run(["git", "add", "--", path], check=True)
    print("The merge conflicted in the files below.  A conflicting hunk with")
    print("an empty `ours` side means the rtems.org repository extended a")
    print("region which this repository deleted.  Such a hunk is declined: an")
    print("upstream build change does not enter the pre-qualified set by")
    print("itself.")
    print()
    print("Declining a hunk keeps `ours`, which leaves the file unchanged.  A")
    print("file whose hunks were all declined therefore does **not** appear in")
    print("the diff of this pull request; there is nothing to show for it.")
    print()
    print("\n".join(report))
    return 1 if blocked else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
