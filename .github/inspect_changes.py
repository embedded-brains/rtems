#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause
""" Provide a command line interface to inspect change sets. """

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

import fnmatch
import logging
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import tomllib

from specitems import CommonMarkContent, get_arguments

_OK = ":white_check_mark:"

_WARNING = ":warning:"

_ERROR = ":x:"

_SKIP = ":heavy_minus_sign:"

# The change set categories.  Only CATEGORY_SOURCE is upstreamable to the
# rtems.org repository.
CATEGORY_SOURCE = "source"

CATEGORY_SPEC = "spec"

CATEGORY_PKG = "pkg"

CATEGORY_CI = "ci"

CATEGORY_UNKNOWN = "unknown"

# Maps a path prefix to a category.  A prefix ending with a slash matches a
# directory and everything below it, otherwise it matches a file exactly.  The
# longest matching prefix wins, so 'spec/build/' overrides 'spec/'.
_CATEGORIES: dict[str, str] = {
    # The RTEMS source tree.  These paths exist in the rtems.org repository and
    # changes to them are upstreamable.
    "CODEOWNERS": CATEGORY_SOURCE,
    "Doxyfile": CATEGORY_SOURCE,
    "LICENSE.md": CATEGORY_SOURCE,
    "README.md": CATEGORY_SOURCE,
    "_clang-format": CATEGORY_SOURCE,
    "bsps/": CATEGORY_SOURCE,
    "contrib/": CATEGORY_SOURCE,
    "cpukit/": CATEGORY_SOURCE,
    "gccdeps.py": CATEGORY_SOURCE,
    "make/": CATEGORY_SOURCE,
    "rtems-bsps": CATEGORY_SOURCE,
    "rtemslogo.png": CATEGORY_SOURCE,
    "spec/build/": CATEGORY_SOURCE,
    "testsuites/": CATEGORY_SOURCE,
    "waf": CATEGORY_SOURCE,
    "wscript": CATEGORY_SOURCE,
    "yaml/": CATEGORY_SOURCE,
    ".gitlab/": CATEGORY_SOURCE,
    # The specification items.  They are not upstreamed.
    "spec/": CATEGORY_SPEC,
    # The packaging and qualification machinery.  These paths are local to the
    # eb repository.
    ".gitignore": CATEGORY_PKG,
    ".mdformat.toml": CATEGORY_PKG,
    "LICENSE.LLVM": CATEGORY_PKG,
    "Makefile": CATEGORY_PKG,
    "build_tools.py": CATEGORY_PKG,
    "config-bsps/": CATEGORY_PKG,
    "config-tools/": CATEGORY_PKG,
    "pyproject.toml": CATEGORY_PKG,
    "spec-pkg-bsps/": CATEGORY_PKG,
    "spec-pkg-tools/": CATEGORY_PKG,
    "specware.yml": CATEGORY_PKG,
    "src/": CATEGORY_PKG,
    "test-logs/": CATEGORY_PKG,
    "testsuites/isvv/": CATEGORY_PKG,
    "testsuites/membench/": CATEGORY_PKG,
    # The build items which register the two eb-only test suites.  Without
    # these the build specification of the rtems.org repository would reference
    # test programs whose sources are not upstreamed.
    "spec/build/testsuites/isvv/": CATEGORY_PKG,
    "spec/build/testsuites/membench/": CATEGORY_PKG,
    "spec/build/testsuites/optisvv.yml": CATEGORY_PKG,
    "spec/build/testsuites/optmembench.yml": CATEGORY_PKG,
    "uv.lock": CATEGORY_PKG,
    # The continuous integration.  All scripts live below .github so that the
    # category of a change to them is unambiguous.
    ".github/": CATEGORY_CI,
}

# Checked from the longest prefix to the shortest one.
_CATEGORY_PREFIXES: tuple[tuple[str, str], ...] = tuple(
    sorted(_CATEGORIES.items(), key=lambda item: len(item[0]), reverse=True))

_C_SUFFIXES = (".c", ".h")

_IGNORE_FILE = ".gitlab/ignore.toml"

# Marks a file generated from specification items.  The formatting of such a
# file is owned by the export, not by clang-format directly.
_GENERATED_MARKER = "RTEMS quality process"

_GENERATED_MARKER_LINES = 64


def _is_generated(worktree: Path, path: str) -> bool:
    """ Returns true, if the file is generated from specification items. """
    try:
        with open(worktree / path, "r", encoding="utf-8",
                  errors="replace") as src:
            for index, line in enumerate(src):
                if _GENERATED_MARKER in line:
                    return True
                if index >= _GENERATED_MARKER_LINES:
                    break
    except OSError:
        pass
    return False


def get_category(path: str) -> str:
    """ Returns the category of the path. """
    for prefix, category in _CATEGORY_PREFIXES:
        if prefix.endswith("/"):
            if path.startswith(prefix):
                return category
        elif path == prefix:
            return category
    return CATEGORY_UNKNOWN


def get_source_prefixes() -> list[str]:
    """ Returns the path prefixes of the upstreamable category. """
    return sorted(prefix for prefix, category in _CATEGORIES.items()
                  if category == CATEGORY_SOURCE)


def _git(*args: str, cwd: Path | str | None = None) -> str:
    return subprocess.check_output(["git", *args],
                                   cwd=cwd,
                                   encoding="utf-8",
                                   stderr=subprocess.DEVNULL)


def _git_ok(*args: str, cwd: Path | str | None = None) -> bool:
    return subprocess.run(["git", *args],
                          cwd=cwd,
                          check=False,
                          capture_output=True).returncode == 0


def _lines(text: str) -> list[str]:
    return [line for line in text.splitlines() if line.strip()]


def _restore(worktree: Path) -> None:
    """ Undoes what a check did to the worktree.  The export and the formatter
    write files in place, and the untracked ones would otherwise pile up from
    one commit to the next. """
    _git("checkout", "--", ".", cwd=worktree)
    subprocess.run(["git", "clean", "-fdq"],
                   cwd=worktree,
                   check=False,
                   capture_output=True)


class _Ignore:
    """ Provides the exclusions of the .gitlab/ignore.toml file. """

    def __init__(self, path: Path) -> None:
        self._patterns: list[tuple[str, frozenset[str]]] = []
        try:
            with open(path, "rb") as src:
                data = tomllib.load(src)
        except FileNotFoundError:
            logging.warning("no ignore file: %s", path)
            return
        default = frozenset(data.get("defaults", {}).get("exclude", []))
        for entries in data.get("ignore", {}).values():
            for entry in entries:
                exclude = frozenset(entry.get("exclude", default))
                for pattern in entry.get("files", []):
                    self._patterns.append((pattern, exclude))

    def is_excluded(self, path: str, check: str) -> bool:
        """ Returns true, if the path is excluded from the check. """
        for pattern, exclude in self._patterns:
            if check in exclude and fnmatch.fnmatch(path, pattern):
                return True
        return False


class _Findings:
    """ Collects the errors and warnings of the inspection. """

    def __init__(self) -> None:
        self.content = CommonMarkContent()
        self.error_count = 0

    def error(self, message: str, detail: str = "") -> None:
        """ Adds an error. """
        self.error_count += 1
        self._add(_ERROR, message, detail)

    def warning(self, message: str, detail: str = "") -> None:
        """ Adds a warning. """
        self._add(_WARNING, message, detail)

    def _add(self, mark: str, message: str, detail: str) -> None:
        self.content.add(f"{mark} {message}")
        if detail:
            self.content.add_code_block(_lines(detail), language="none")


def _get_commits(base_ref: str, head_ref: str,
                 exclude_ref: str | None) -> list[tuple[str, str]]:
    # Commits which are already in the rtems.org repository are not the
    # responsibility of this repository.  Excluding them keeps the merge pull
    # requests of Harmonia from being judged by the rules of the eb repository.
    exclude = [f"^{exclude_ref}"] if exclude_ref else []
    log = _git("log", "--no-merges", "--reverse", "--format=format:%H %s",
               f"{base_ref}..{head_ref}", *exclude)
    commits = []
    for line in _lines(log):
        commit, _, subject = line.partition(" ")
        commits.append((commit, subject))
    return commits


def _get_files(commit: str) -> list[str]:
    return sorted(
        _lines(
            _git("diff-tree", "--no-commit-id", "--name-only", "-r", commit)))


def _get_existing_files(commit: str, files: list[str]) -> list[str]:
    return [f for f in files if _git_ok("cat-file", "-e", f"{commit}:{f}")]


def _check_invariant(upstream_ref: str, findings: _Findings) -> None:
    """ Checks that the category table matches the two trees. """
    for prefix in get_source_prefixes():
        path = prefix.rstrip("/")
        if not _git_ok("cat-file", "-e", f"{upstream_ref}:{path}"):
            findings.error(
                f"The path prefix `{prefix}` has the category "
                f"`{CATEGORY_SOURCE}`, but it does not exist in "
                f"`{upstream_ref}`.  Upstreaming it would push content to the "
                "rtems.org repository which does not belong there.")
    for line in _lines(_git("ls-tree", "HEAD")):
        info, _, name = line.partition("\t")
        kind = info.split()[1]
        path = f"{name}/" if kind == "tree" else name
        if get_category(path) == CATEGORY_UNKNOWN:
            findings.error(
                f"The top-level path `{name}` has no category.  Add it to "
                "`_CATEGORIES` in `inspect_changes.py`.")


def _check_spec_format(worktree: Path, items: list[str],
                       findings: _Findings, url: str) -> str:
    if not items:
        return _SKIP
    clang_format = shutil.which("clang-format")
    if clang_format is None:
        findings.error("The clang-format tool is not available.")
        return _ERROR
    subprocess.run([
        "specverify", "--format-items", f"--clang-format-path={clang_format}",
        "--clang-format-style=default:file:_clang-format",
        "--do-not-indent-lists", *items
    ],
                   cwd=worktree,
                   check=False,
                   capture_output=True)
    unformatted = _lines(_git("diff", "--name-only", "--", *items,
                              cwd=worktree))
    _restore(worktree)
    if unformatted:
        findings.error(
            f"In {url}, these specification items are not formatted:",
            "\n".join(unformatted))
        return _ERROR
    return _OK


def _check_c_format(worktree: Path, files: list[str],
                    findings: _Findings, url: str) -> str:
    if not files:
        return _SKIP
    result = subprocess.run(
        ["clang-format", "--style=file", "--dry-run", "-Werror", *files],
        cwd=worktree,
        check=False,
        capture_output=True,
        encoding="utf-8")
    if result.returncode != 0:
        findings.error(f"In {url}, these C language files are not formatted:",
                       result.stderr)
        return _ERROR
    return _OK


def _check_export(worktree: Path, items: list[str], findings: _Findings,
                  url: str) -> str:
    """ Checks that the files exported from the items of the commit are part
    of it.  Only the items of the commit are exported, so the export debt of
    the rest of the tree is not reported here. """
    if not items:
        return _SKIP
    # Do not format the exported code.  The committed files match the plain
    # export; formatting here would report most of the tree as stale.
    try:
        result = subprocess.run(["specwareexport", *items],
                                cwd=worktree,
                                check=False,
                                capture_output=True,
                                encoding="utf-8")
    except FileNotFoundError:
        findings.error("The specwareexport tool is not available, so the "
                       "exported files were not checked.")
        return _ERROR
    if result.returncode != 0:
        findings.error(
            f"In {url}, the export failed.  A specification item file may be "
            "associated with no item:", result.stderr or result.stdout)
        _restore(worktree)
        return _ERROR
    # Compare the tracked files only.  The export also writes files which this
    # repository does not commit, such as the Doxygen input.
    stale = _lines(_git("diff", "--name-only", cwd=worktree))
    _restore(worktree)
    if stale:
        findings.error(
            f"In {url}, these exported files are missing from the commit or "
            "out of date:", "\n".join(stale))
        return _ERROR
    return _OK


def _check_extractable(repository: Path, upstream_ref: str,
                       commits: list[tuple[str, str]],
                       categories: dict[str, list[str]],
                       findings: _Findings) -> None:
    """ Checks that the upstreamable commits apply to the upstream branch. """
    source = [
        commit for commit, _ in commits
        if categories[commit] == [CATEGORY_SOURCE]
    ]
    if not source:
        return
    logging.info("check that %d upstreamable commits apply to %s", len(source),
                 upstream_ref)
    with tempfile.TemporaryDirectory() as tmp_dir:
        worktree = Path(tmp_dir) / "extract"
        _git("worktree", "add", "--detach", str(worktree), upstream_ref,
             cwd=repository)
        try:
            for commit in source:
                if not _git_ok("cherry-pick", "--no-commit", commit,
                               cwd=worktree):
                    conflicts = _lines(
                        _git("diff", "--name-only", "--diff-filter=U",
                             cwd=worktree))
                    subprocess.run(["git", "cherry-pick", "--quit"],
                                   cwd=worktree,
                                   check=False,
                                   capture_output=True)
                    findings.error(
                        f"The commit `{commit[:10]}` does not apply to "
                        f"`{upstream_ref}`.  Merge `{upstream_ref}` into the "
                        "branch and resolve the conflicts in:",
                        "\n".join(conflicts))
                    return
                _git("commit", "--no-edit", "--allow-empty", "-C", commit,
                     cwd=worktree)
        finally:
            subprocess.run(
                ["git", "worktree", "remove", "--force",
                 str(worktree)],
                cwd=repository,
                check=False,
                capture_output=True)


def _get_repository_path() -> Path:
    repository = Path(".").absolute()
    while not (repository / ".git").exists():
        if repository.parent == repository:
            raise RuntimeError("no Git repository found")
        repository = repository.parent
    return repository


def main(argv: list[str]) -> int:
    """ Inspects the change set of a pull request. """

    def _add_arguments(parser):
        parser.add_argument("--upstream-ref",
                            help="the reference of the rtems.org repository "
                            "mirror (default: rtems.org/main)",
                            default="rtems.org/main")
        parser.add_argument("--output",
                            help="write the report to this file instead of "
                            "the standard output")
        parser.add_argument("url",
                            metavar="URL",
                            nargs=1,
                            help="the repository URL")
        parser.add_argument("base_ref",
                            metavar="BASE_REF",
                            nargs=1,
                            help="the base Git reference")
        parser.add_argument("head_ref",
                            metavar="HEAD_REF",
                            nargs=1,
                            help="the head Git reference")

    args = get_arguments(argv[1:],
                         description=sys.modules[__name__].__doc__,
                         add_arguments=(_add_arguments, ))
    repository = _get_repository_path()
    base_ref = args.base_ref[0]
    head_ref = args.head_ref[0]
    url = args.url[0]
    findings = _Findings()
    ignore = _Ignore(repository / _IGNORE_FILE)
    has_upstream = _git_ok("rev-parse", "--verify", f"{args.upstream_ref}^{{commit}}")
    if has_upstream:
        _check_invariant(args.upstream_ref, findings)
    else:
        findings.warning(
            f"The upstream reference `{args.upstream_ref}` does not exist.  "
            "The category invariant and the extractability check are skipped.")
    commits = _get_commits(base_ref, head_ref,
                           args.upstream_ref if has_upstream else None)
    logging.info("inspect %d commits in %s..%s", len(commits), base_ref,
                 head_ref)
    rows = [["Subject", "Category", "Format", "Export", "Status"]]
    categories: dict[str, list[str]] = {}
    with tempfile.TemporaryDirectory() as tmp_dir:
        worktree = Path(tmp_dir) / "inspect"
        _git("worktree", "add", "--detach", str(worktree), head_ref,
             cwd=repository)
        try:
            for commit, subject in commits:
                commit_url = f"{url}/commit/{commit}"
                files = _get_files(commit)
                found = sorted({get_category(f) for f in files})
                categories[commit] = found
                errors_before = findings.error_count
                if len(found) != 1 or CATEGORY_UNKNOWN in found:
                    findings.error(
                        f"In {commit_url}, the change set belongs to more "
                        f"than one category: {', '.join(found)}.  Split it "
                        "into one commit per category.")
                _git("checkout", "--detach", commit, cwd=worktree)
                existing = _get_existing_files(commit, files)
                items = [
                    f for f in existing
                    if f.startswith("spec/") and f.endswith(".yml")
                ]
                c_files = [
                    f for f in existing
                    if f.endswith(_C_SUFFIXES)
                    and not ignore.is_excluded(f, "format")
                    and not _is_generated(worktree, f)
                ]
                spec_status = _check_spec_format(worktree, items, findings,
                                                 commit_url)
                c_status = _check_c_format(worktree, c_files, findings,
                                           commit_url)
                export = _check_export(worktree, items, findings, commit_url)
                fmt = _ERROR if _ERROR in (spec_status, c_status) else (
                    _OK if _OK in (spec_status, c_status) else _SKIP)
                status = (_OK if findings.error_count == errors_before else
                          _ERROR)
                rows.append([
                    f"[{subject}]({commit_url})", ", ".join(found), fmt,
                    export, status
                ])
        finally:
            subprocess.run(
                ["git", "worktree", "remove", "--force",
                 str(worktree)],
                cwd=repository,
                check=False,
                capture_output=True)
    if has_upstream:
        _check_extractable(repository, args.upstream_ref, commits, categories,
                           findings)
    content = CommonMarkContent()
    content.add_simple_table(rows)
    content.add(findings.content)
    report = str(content)
    if args.output:
        Path(args.output).write_text(report, encoding="utf-8")
    else:
        sys.stdout.write(report)
    return 1 if findings.error_count else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
