#!/bin/bash
# SPDX-License-Identifier: MIT
# -----------------------------------------------------------------------------
# Copyright (c) 2025 Arm Limited
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the 'Software'), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# -----------------------------------------------------------------------------
set -e

printf "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = \n"
printf "Running pycodestyle\n"
printf "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = \n"
printf "\n"
python3 -m pycodestyle --config .pycodestyle.ini ./lgcpy_*.py ./lgcpy
if [ $? -eq 0 ]; then
    echo "Success: no issues found"
fi

printf "\n"
printf "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = \n"
printf "Running mypy\n"
printf "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = \n"
printf "\n"
python3 -m mypy ./lgcpy_*.py ./lgcpy

printf "\n"
printf "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = \n"
printf "Running pylint\n"
printf "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = \n"
printf "\n"
python3 -m pylint --rcfile=.pylintrc ./lgcpy_*.py ./lgcpy
