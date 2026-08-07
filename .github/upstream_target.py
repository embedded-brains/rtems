#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause
""" Resolve the GitLab push target of an eb repository maintainer. """

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

import argparse
from pathlib import Path
import sys

import yaml

# The GitLab personal access token ('write_repository' scope) must never be
# able to reach any other host.  The mapping file comes from the base branch
# and is therefore trusted, but a typo must not turn into a credential sent
# somewhere else.
_HOST = "https://gitlab.rtems.org/"

_MAP = ".github/upstreaming-maintainers.yml"


def main(argv: list[str]) -> int:
    """ Prints the push target of the maintainer as key=value lines. """
    parser = argparse.ArgumentParser(description=sys.modules[__name__].__doc__)
    parser.add_argument("--map",
                        default=_MAP,
                        help=f"the maintainer map (default: {_MAP})")
    parser.add_argument("login", metavar="LOGIN", help="the GitHub login")
    args = parser.parse_args(argv[1:])
    with open(args.map, "r", encoding="utf-8") as src:
        maintainers = yaml.safe_load(src).get("maintainers") or {}
    entry = maintainers.get(args.login)
    if entry is None:
        known = ", ".join(sorted(maintainers)) or "none"
        print(
            f"'{args.login}' is not configured for upstreaming.  Add an entry "
            f"to '{args.map}' and a matching repository secret.  Configured: "
            f"{known}.",
            file=sys.stderr)
        return 1
    fork = str(entry.get("fork", ""))
    secret = str(entry.get("secret", ""))
    if not fork.startswith(_HOST):
        print(f"the fork of '{args.login}' is not on {_HOST}: {fork}",
              file=sys.stderr)
        return 1
    if not secret:
        print(f"the entry of '{args.login}' has no secret name",
              file=sys.stderr)
        return 1
    print(f"fork={fork}")
    print(f"secret={secret}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
