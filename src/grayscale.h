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

#ifndef GRAYSCALE_H
#define GRAYSCALE_H

#include <vips/vips.h>

#include <stdbool.h>

bool compareFloats(float f1, float f2);
bool isBasicallyGrayImage(VipsImage *image);
int stripToGrayscale(VipsImage *image, VipsImage **imageGrayed);

#ifndef COMPAREFLOATPRECISION
#define COMPAREFLOATPRECISION 0.00001
#endif

#endif /* GRAYSCALE_H */
