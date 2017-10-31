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

#include "grayscale.h"

bool compareFloats(float f1, float f2) {
  float precision = 0.00001;
  if (((f1 - precision) < f2) && ((f1 + precision) > f2)) {
    return true;
  } else {
    return false;
  }
}

bool isBasicallyGrayImage(VipsImage *image) {
  VipsInterpretation imInter = 0;

  VipsImage *redChannel = NULL;
  VipsImage *greenChannel = NULL;
  VipsImage *blueChannel = NULL;

  double redAvg = 0.0;
  double greenAvg = 0.0;
  double blueAvg = 0.0;

  imInter = vips_image_get_interpretation(image);
  switch (imInter) {
  case VIPS_INTERPRETATION_sRGB:
  case VIPS_INTERPRETATION_scRGB:
  case VIPS_INTERPRETATION_RGB16:
    // There's a chance it's basically grayscale.
    break;
  case VIPS_INTERPRETATION_B_W:
  case VIPS_INTERPRETATION_GREY16:
    // It's already gray.
    return false;
  default:
    // Something weird is up.
    return false;
  }
  vips_extract_band(image, &redChannel, 0, NULL);
  vips_extract_band(image, &greenChannel, 1, NULL);
  vips_extract_band(image, &blueChannel, 2, NULL);

  vips_avg(redChannel, &redAvg, NULL);
  vips_avg(greenChannel, &greenAvg, NULL);
  vips_avg(blueChannel, &blueAvg, NULL);

  g_object_unref(redChannel);
  g_object_unref(greenChannel);
  g_object_unref(blueChannel);

  if (compareFloats(redAvg, greenAvg) && compareFloats(redAvg, blueAvg)) {
    return true;
  } else {
    return false;
  }
}

int stripToGrayscale(VipsImage *image, VipsImage **imageGrayed) {
  switch (vips_image_get_interpretation(image)) {
  case VIPS_INTERPRETATION_sRGB:
  case VIPS_INTERPRETATION_scRGB:
    vips_colourspace(image, imageGrayed, VIPS_INTERPRETATION_B_W, NULL);
    break;
  case VIPS_INTERPRETATION_RGB16:
    vips_colourspace(image, imageGrayed, VIPS_INTERPRETATION_GREY16, NULL);
    break;
  default:
    fprintf(stderr, "Something is horribly wrong with this colourspace. \n");
    return 0;
    break;
  }
  return 1;
}
