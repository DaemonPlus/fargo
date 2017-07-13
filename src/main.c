/*
    fargo - Removes unseeable and ghost alpha from PNG images, optimized.
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

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int displayHelp(char **argv);

int main(int argc, char **argv) {
  int opt = 0;
  char *inputFile = NULL;
  char *outputFile = NULL;
  bool clobber = false;
  bool simulatedRun = false;
  bool verbose = false;
  bool quietMode = false;
  double thresholdFactor = DEFAULTTHRESHOLDFACTOR;

  while ((opt = getopt(argc, argv, ":i:o:f:hvcsq")) != -1) {
    switch (opt) {
    case 'i':
      inputFile = optarg;
      struct stat buffer;
      if (!(stat(inputFile, &buffer) == 0) || (!(S_ISREG(buffer.st_mode))))
        exit(-1);
      char *inFileExt = strrchr(optarg, '.');
      for (int i = 0; inFileExt[i] != '\0'; i++)
        inFileExt[i] = tolower(inFileExt[i]);
      if (!(strcmp(inFileExt, ".png") == 0))
        exit(-1);
      break;
    case 'o':
      outputFile = optarg;
      break;
    case 'f':
      thresholdFactor = strtod(optarg, NULL);
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

  if (inputFile == NULL) {
    displayHelp(argv);
  }

  if (thresholdFactor > 1 || thresholdFactor < 0) {
    printf("Threshold factor has to be in between 0 and 1!\n");
    exit(-1);
  }

  if (clobber) {
    outputFile = inputFile;
  }

  if (verbose) {
    printf("Verbose mode on!\n");
    printf("Input File: %s\n", inputFile);
    printf("Output File: %s\n", outputFile);
    printf("Threshold Factor: %f\n", thresholdFactor);

    if (simulatedRun) {
      printf("Simulated run on!\n");
    }
    if (clobber) {
      printf("Clobber on!\n");
    }
  }

  VipsImage *imageIn = NULL;
  VipsImage *alphaChannel = NULL;
  VipsImage *imageOut = NULL;
  double finalMean = 0.0; // ૮( ᵒ̌皿ᵒ̌ )ა
  VipsInterpretation imInter = 0;

  // Start vips
  VIPS_INIT(argv[0]);

  // Read the image.
  imageIn = vips_image_new_from_file(inputFile, NULL);

  // Check if it even has an Alpha
  if (!vips_image_hasalpha(imageIn)) {
    exit(0);
  }

  imInter = vips_image_get_interpretation(imageIn);
  switch (imInter) {
  case VIPS_INTERPRETATION_sRGB:
  case VIPS_INTERPRETATION_scRGB:
    vips_extract_band(imageIn, &alphaChannel, 3, NULL);
    vips_avg(alphaChannel, &finalMean, NULL);
    finalMean = finalMean / 255;
    break;
  case VIPS_INTERPRETATION_B_W:
    vips_extract_band(imageIn, &alphaChannel, 1, NULL);
    vips_avg(alphaChannel, &finalMean, NULL);
    finalMean = finalMean / 255;
    break;
  case VIPS_INTERPRETATION_RGB16:
    vips_extract_band(imageIn, &alphaChannel, 3, NULL);
    vips_avg(alphaChannel, &finalMean, NULL);
    finalMean = finalMean / 65536;
    break;
  case VIPS_INTERPRETATION_GREY16:
    vips_extract_band(imageIn, &alphaChannel, 1, NULL);
    vips_avg(alphaChannel, &finalMean, NULL);
    finalMean = finalMean / 65536;
    break;
  default:
    exit(123);
    break;
  }

  if (finalMean == 1) {
    printf("empty alpha, removing - %s\n", inputFile);
    if (!simulatedRun) {
      // Deactivate alpha channel.
      vips_flatten(imageIn, &imageOut, 0, NULL);

      // Write the cleaned image to disk.
      vips_image_write_to_file(imageOut, outputFile, NULL);
    }
  } else if (finalMean > thresholdFactor) {
    printf("pointless alpha - mean: %f - %s\n", finalMean, inputFile);
    if (outputFile != NULL && !simulatedRun) {
      // Deactivate alpha channel.
      vips_flatten(imageIn, &imageOut, 0, NULL);

      // Write the cleaned image to disk.
      vips_image_write_to_file(imageOut, outputFile, NULL);
    }
  } else if (!quietMode) {
    printf("needed alpha - mean: %f - %s\n", finalMean, inputFile);
  }

  // Clean up
  vips_shutdown();

  return 0;
}

int displayHelp(char **argv) {
  fprintf(
      stderr,
      "Usage: %s [-h -v -c -s -q] -i image.png -o output.png\nSee 'man fargo' "
      "for more information.\n",
      argv[0]);
  exit(0);
}
