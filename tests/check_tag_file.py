import argparse
import fileinput
import logging
import os
import pathlib
import re
import subprocess
import tempfile

import typeguard

SOURCE_CODE_HEADER = \
"""
void useless();
"""

@typeguard.typechecked
def parse_args() -> argparse.Namespace:
    """
    Parse CLI arguments.
    """
    parser = argparse.ArgumentParser()

    parser.add_argument('--tag-file', type = pathlib.Path, required = True)
    parser.add_argument('--doxygen',  type = pathlib.Path, required = False, default = pathlib.Path(os.environ.get('Doxygen_ROOT')) / 'bin' / 'doxygen')

    return parser.parse_args()

@typeguard.typechecked
def check_tag_file(*, file : pathlib.Path, doxygen : pathlib.Path) -> None:
    """
    Check that the `Doxygen` tag `file` is valid.
    """
    logging.info(f"Checking that the tag file {file} is valid.")

    with tempfile.TemporaryDirectory(delete = True) as tmpdir_str:
        tmpdir = pathlib.Path(tmpdir_str)

        hpp = tmpdir / 'test.hpp'
        hpp.write_text(SOURCE_CODE_HEADER)

        doxyfile = tmpdir / 'doxygen'
        subprocess.check_call([doxygen, '-g', doxyfile])

        replacing = {
            r'WARN_AS_ERROR[ ]+=[ ]+NO' : [False, 'WARN_AS_ERROR=YES'],
            r'INPUT[ ]+='               : [False, f"INPUT={hpp}"],
            r'TAGFILES[ ]+='            : [False, f'TAGFILES={file}'],
        }

        with fileinput.input(doxyfile, inplace = True) as f:
            for line in f:
                for k, v in replacing.items():
                    if v[0] is False:
                        if re.match(k, line) is not None:
                            v[0] = True
                            line = v[1]
                            break
                print(line)

        if any(v[0] is False for v in replacing.values()):
            raise RuntimeError("Couldn't replace all the needed parameters.")

        subprocess.check_call([doxygen, doxyfile], cwd = tmpdir)

if __name__ == '__main__':

    logging.basicConfig(level = logging.INFO)

    args = parse_args()

    logging.info(f"Received arguments: {args}")

    check_tag_file(file = args.tag_file, doxygen = args.doxygen)
