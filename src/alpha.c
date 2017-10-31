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

#include "alpha.h"

double getMeanAlpha(VipsImage *image) {
  VipsImage *alphaChannel = NULL;
  double mean = 0.0;

  switch (vips_image_get_interpretation(image)) {
  case VIPS_INTERPRETATION_sRGB:
  case VIPS_INTERPRETATION_scRGB:
    vips_extract_band(image, &alphaChannel, 3, NULL);
    vips_avg(alphaChannel, &mean, NULL);
    g_object_unref(alphaChannel);
    return (mean / 255);
  case VIPS_INTERPRETATION_B_W:
    vips_extract_band(image, &alphaChannel, 1, NULL);
    vips_avg(alphaChannel, &mean, NULL);
    g_object_unref(alphaChannel);
    return (mean / 255);
  case VIPS_INTERPRETATION_RGB16:
    vips_extract_band(image, &alphaChannel, 3, NULL);
    vips_avg(alphaChannel, &mean, NULL);
    g_object_unref(alphaChannel);
    return (mean / 65536);
  case VIPS_INTERPRETATION_GREY16:
    vips_extract_band(image, &alphaChannel, 1, NULL);
    vips_avg(alphaChannel, &mean, NULL);
    g_object_unref(alphaChannel);
    return (mean / 65536);
  default:
    fprintf(stderr, "Something is horribly wrong with this colourspace. \n");
    return -1;
  }
}
