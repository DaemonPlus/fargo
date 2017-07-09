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

#if IMAGEMAGICKVERSION == 6
#include <wand/MagickWand.h>
#else // assume it's 7+
#include <MagickWand/MagickWand.h>
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ThrowWandException(wand)                                               \
  {                                                                            \
    char *description;                                                         \
                                                                               \
    ExceptionType severity;                                                    \
                                                                               \
    description = MagickGetException(wand, &severity);                         \
    (void)fprintf(stderr, "%s %s %lu %s\n", GetMagickModule(), description);   \
    description = (char *)MagickRelinquishMemory(description);                 \
    exit(-1);                                                                  \
  }

int displayHelp(char **argv);

int main(int argc, char **argv) {
  int opt = 0;
  char *inputFile = NULL;
  char *outputFile = NULL;
  bool clobber = false;
  bool simulatedRun = false;
  bool verbose = false;
  double thresholdFactor = 0.95;

  while ((opt = getopt(argc, argv, ":i:o:f:hvcs")) != -1) {
    switch (opt) {
    case 'i':
      inputFile = optarg;
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
    exit(0);
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

  MagickBooleanType status;
  MagickWand *magick_wand;

#if IMAGEMAGICKVERSION == 6
  double doubleZeroMean = 0.0;
  double doubleZeroStan = 0.0;
  double *mean = &doubleZeroMean;
  double *standard_deviation = &doubleZeroStan;
#else
  ChannelStatistics *channel_statistics = NULL;
#endif

  double finalMean = 0.0; // ૮( ᵒ̌皿ᵒ̌ )ა

  // Read the image.
  MagickWandGenesis();
  magick_wand = NewMagickWand();
  status = MagickReadImage(magick_wand, inputFile);
  if (status == MagickFalse)
    ThrowWandException(magick_wand);

  // Check if it even has an Alpha
  if (MagickGetImageAlphaChannel(magick_wand)) {
// ImageMagick version specific functions for getting the mean Alpha value.
#if IMAGEMAGICKVERSION == 6
    MagickGetImageChannelMean(magick_wand, OpacityChannel, mean,
                              standard_deviation);
    finalMean = *mean;
#else
    channel_statistics = MagickGetImageStatistics(magick_wand);
    finalMean = channel_statistics[AlphaPixelChannel].mean;
    MagickRelinquishMemory(channel_statistics);
#endif

    if (finalMean == QUANTUM_DEPTH_MAXSIZE) {
      printf("%s has an empty alpha, automatically removing and overwriting.\n",
             inputFile);
      if (!simulatedRun) {
        // Deactivate "Matte" (Alpha channel)
        status = MagickSetImageMatte(magick_wand, MagickFalse);
        if (status == MagickFalse)
          ThrowWandException(magick_wand);

        // Write the cleaned image to disk.
        status = MagickWriteImage(magick_wand, inputFile);
      }

    } else if (finalMean > (QUANTUM_DEPTH_MAXSIZE * thresholdFactor)) {
      printf("Mean: %f - %s - Probably a pointless alpha.\n", finalMean,
             inputFile);
      if (outputFile != NULL && !simulatedRun) {

        // Deactivate "Matte" (Alpha channel)
        status = MagickSetImageMatte(magick_wand, MagickFalse);
        if (status == MagickFalse)
          ThrowWandException(magick_wand);

        // Write the cleaned image to disk.
        status = MagickWriteImage(magick_wand, outputFile);
      }

    } else {
      printf("Mean: %f - %s - Needed alpha.\n", finalMean, inputFile);
    }
  }
  if (status == MagickFalse)
    ThrowWandException(magick_wand);

  // Clean up
  magick_wand = DestroyMagickWand(magick_wand);
  MagickWandTerminus();
  return 0;
}

int displayHelp(char **argv) {
  fprintf(stderr,
          "Usage: %s [-h -v -c -s] -i image.png -o output.png\nSee 'man fargo' "
          "for more information.\n",
          argv[0]);
  exit(0);
}
