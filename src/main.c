/*
    fargo - Removes unseeable or ghost channels from PNG images, optimized.
    Copyright 2017 Daemon Lee Schmidt

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <vips/vips.h>

#ifndef DEFAULTTHRESHOLDFACTOR
#define DEFAULTTHRESHOLDFACTOR 0.975
#endif

#include <unistd.h>

#include "alpha.h"
#include "grayscale.h"
#include "util.h"

int main(int argc, char **argv) {
  int opt = 0;
  char *inputFile = NULL;
  char *outputFile = NULL;

  bool grayscaleTesting = false;
  bool imageIsGray = false;
  bool imageHasAlpha = false;

  bool clobber = false;
  bool simulatedRun = false;
  bool verbose = false;
  bool quietMode = false;
  double thresholdFactor = DEFAULTTHRESHOLDFACTOR;

  while ((opt = getopt(argc, argv, ":i:o:f:ghvcsq")) != -1) {
    switch (opt) {
    case 'i':
      inputFile = optarg;
      break;
    case 'o':
      outputFile = optarg;
      break;
    case 'f':
      thresholdFactor = strtod(optarg, NULL);
      if (thresholdFactor > 1 || thresholdFactor < 0) {
        printf("Threshold factor has to be in between 0 and 1!\n");
        exit(-1);
      }
      break;
    case 'g':
      grayscaleTesting = true;
      break;
    case 'h':
      displayHelp(argv);
      break;
    case 'v':
      verbose = true;
      break;
    case 'c':
      clobber = true;
      break;
    case 's':
      simulatedRun = true;
      break;
    case 'q':
      quietMode = true;
      break;
    default: // '?'
      displayHelp(argv);
      break;
    }
  }

  if (inputFile == NULL)
    displayHelp(argv);
  if (clobber)
    outputFile = inputFile;

  if (verbose) {
    printf("Input File: %s\n"
           "Output File: %s\n"
           "Threshold Factor: %f\n",
           inputFile, outputFile, thresholdFactor);

    if (grayscaleTesting)
      printf("Grayscale testing on!\n");

    if (simulatedRun)
      printf("Simulated run on!\n");

    if (clobber)
      printf("Clobber on!\n");
  }

  VipsImage *imageIn = NULL;
  VipsImage *imageOut = NULL;
  VipsImage *imageTemp = NULL;
  double finalMean = 0.0; // ૮( ᵒ̌皿ᵒ̌ )ა
  bool imageChanged = false;

  // Start vips
  if (VIPS_INIT(argv[0]))
    vips_error_exit(NULL);

  // Read the image.
  if (!(imageIn = vips_image_new_from_file(inputFile, NULL)))
    vips_error_exit(NULL);

  // Test for gray-ness if flag is enabled
  if (grayscaleTesting && isBasicallyGrayImage(imageIn)) {
    imageIsGray = true;
    if (stripToGrayscale(imageIn, &imageTemp))
      vips_copy(imageTemp, &imageIn, NULL);
    g_object_unref(imageTemp);
    vips_copy(imageIn, &imageOut, NULL);
    imageChanged = true;
  }

  // Check if it even has an Alpha
  if (vips_image_hasalpha(imageIn)) {
    imageHasAlpha = true;
    finalMean = getMeanAlpha(imageIn);
  }

  if (imageHasAlpha) {
    if (finalMean == 1 && !quietMode) {
      printf("empty alpha (removing) - ");
    } else if (finalMean > thresholdFactor) {
      printf("pointless alpha - mean: %f - ", finalMean);
    } else {
      printf("needed alpha - mean: %f - ", finalMean);
    }
  }

  if (imageIsGray)
    printf("unneeded RGB - ");

  if (imageHasAlpha || imageIsGray)
    printf("%s\n", inputFile);

  if (finalMean == 1 && !simulatedRun) {
    vips_flatten(imageIn, &imageOut, 0, NULL);
    imageChanged = true;
    outputFile = inputFile;
  } else if (finalMean > thresholdFactor) {
    // Deactivate alpha channel.
    vips_flatten(imageIn, &imageOut, 0, NULL);
    imageChanged = true;
  }

  // Write the cleaned image to disk.
  if (imageChanged && outputFile != NULL && !simulatedRun) {
    if (vips_image_write_to_file(imageOut, outputFile, NULL))
      vips_error_exit(NULL);
  }

  // Clean up
  if (imageOut != NULL)
    g_object_unref(imageOut);
  g_object_unref(imageIn);
  vips_shutdown();

  return 0;
}
