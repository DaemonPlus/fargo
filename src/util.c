/*
    This file is part of fargo.

    Fargo is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Fargo is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with fargo.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "util.h"

int displayHelp(char **argv) {
  fprintf(stderr,
          "Usage: %s [-h -g -v -c -s -q] -i image.png -o output.png\nSee 'man "
          "fargo' "
          "for more information.\n",
          argv[0]);
  exit(0);
}
