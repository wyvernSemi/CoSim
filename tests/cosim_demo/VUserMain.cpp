// ------------------------------------------------------------------------------
//
//  File Name:           VUserMain.cpp
//  Design Unit Name:    Co-simulation demo program
//  Revision:            OSVVM MODELS STANDARD VERSION
//
//  Maintainer:          Simon Southwell      email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell   simon.southwell@gmail.com
//
//  Description:
//      Co-simulation test transaction source
//
//  Developed by:
//        Simon Southwell
//
//  Revision History:
//    Date      Version    Description
//    07/2026   2026       Initial revision
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2026 by Simon Southwell
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
// ------------------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>

// Import VProc user API
#include "OsvvmCosim.h"

#include "jfif.h"
#include "jfif_gtk.h"
#include "bitmap.h"


#ifdef __cplusplus
#define EXTLINKAGE extern "C"
#else
#define EXTLINKAGE
#endif

// ------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------

static const uint32_t NUM_OF_IMAGES                = 2;
static const uint32_t IMG_BASE_ADDR[NUM_OF_IMAGES] = {0x10000000, 0x20000000};
static const uint32_t PAGE_SIZE                    = 1024;
static const uint32_t MAX_IMG_SIZE_KBYTES          = 256;
static const uint32_t STRBUFSIZE                   = 80;

static const char     fnames[NUM_OF_IMAGES][STRBUFSIZE] = {
		                  "../../OsvvmLibraries/CoSim/tests/cosim_demo/demo_image1.jpg",
                          "../../OsvvmLibraries/CoSim/tests/cosim_demo/demo_image2.jpg"
                      };

// ------------------------------------------------------------------------------
// Local shared variables
// ------------------------------------------------------------------------------

// 128K byte image buffer as 1Kbyte pages of 32-bit words
static uint32_t imgbuf [MAX_IMG_SIZE_KBYTES][PAGE_SIZE/4];

// The image byte count passed from node 1 to node 0
static uint32_t bytecount[NUM_OF_IMAGES];

// Synchronising flag between node 1 and node 0 to
// indicate data loaded to AxiMemory
static unsigned mem_init_sync = 0;

// ------------------------------------------------------------------------------
// Main entry point for node 0 software
// ------------------------------------------------------------------------------

EXTLINKAGE void VUserMain0 (int node)
{
    // Indicate this node's program has started
    VPrint("VUserMain0(): node=%d\n", node);

    int localsync = 0;

    // JFIF data pointers
    uint8_t*    obuf;
    uint8_t*    databuf;

    // Command line arguments
    std::string arg1("jfif : GTK+ : OSVVM");
    char*       argv[1];
    int         argc    = 1;

    // Error status
    bool        error   = false;

    // Page counter
    int         page;

    // Address incrementer
    uint32_t    address;

    // Create a cosim API object
    OsvvmCosim  cosim(node);

    // Give a program name string argument to display in the GTK window
    argv[0] = new char[STRBUFSIZE];
    strcpy(argv[0], "jfif : GTK+ : OSVVM");

    for (int fidx = 0; fidx < NUM_OF_IMAGES; fidx++)
    {

        // Wait for memory to be initialised with file data
        while (mem_init_sync == localsync)
        {
            cosim.tick(1, false, error);
        }
        
        mem_init_sync++;
        localsync = mem_init_sync;

        // Number of pages in image, rounded down
        int num_of_pages = bytecount[fidx] / PAGE_SIZE;
        
        address = IMG_BASE_ADDR[fidx];

        ///////////////////////////////////////////////////////////////
        // Do page size burst reads to fetch image data from AxiMemory
        ///////////////////////////////////////////////////////////////
        for (page = 0; page < num_of_pages; page++)
        {
            cosim.transBurstRead(address, imgbuf[page], PAGE_SIZE);
            address += PAGE_SIZE;
        }

        // If there is a tail of bytes, read them in now
        if (bytecount[fidx] % PAGE_SIZE)
        {
            cosim.transBurstRead(address, imgbuf[page], bytecount[fidx] % PAGE_SIZE);
            address += bytecount[fidx]%PAGE_SIZE;
        }

        // Flatten image via a byte pointer
        uint8_t* imgbytes = (uint8_t*)imgbuf;

        ///////////////////////////////////////////////////////////////
        // Decode the JPEG image into bitmap using the jfif library
        ///////////////////////////////////////////////////////////////

        int status = jpeg_process_jfif_c
                     (
                          imgbytes,   // JPEG image byte buffer input
                          &obuf,      // Returned pointer to decode BMP image
                          &databuf,   // Returned pointer to the raw data part of the image
                          0           // No debug output
                     );

        // Point to the decoded bitmap header to extract the image dimensions
        bmhdr_t* bmp_hdr = (bmhdr_t *)obuf;

        ///////////////////////////////////////////////////////////////
        // Display the decoded image in a GTK+ window
        ///////////////////////////////////////////////////////////////

        jpeg_display_img_data
		(
				argc,
				argv,
				databuf,
				BMP_SWPEND32(bmp_hdr->i.biWidth),
				BMP_SWPEND32(bmp_hdr->i.biHeight)
		);
        
        delete obuf;
        delete databuf;
    }

    // Flag that the program has finished
    cosim.tick(10, true, error);

    // If ever got this far then sleep forever
    SLEEPFOREVER;
}

// ------------------------------------------------------------------------------
// Main entry point for node 1 virtual processor software
// ------------------------------------------------------------------------------

extern "C" void VUserMain1(int node)
{
    // Indicate this node's program has started
    VPrint("VUserMain1(): node=%d\n", node);

    int localsync = 0;

    // Error status
    bool        error = false;

    // Address incrementer
    uint32_t    address;

    // Input byte from file
    int         byte;

    // 32-bit word accumulator
    uint32_t    word;

    // Create a cosim API object
    OsvvmCosim  cosim(node);

    for (int fidx = 0; fidx < NUM_OF_IMAGES; fidx++)
    {

        // Open a JPEG file to process
        FILE* img_fp = fopen(fnames[fidx], "rb");

        ///////////////////////////////////////////////////////////////
        // Read in bytes from file and accumulate into a word, writing
        // result to AxiMemory over MIT bus
        ///////////////////////////////////////////////////////////////
        
        address = IMG_BASE_ADDR[fidx];

        while ((byte = getc(img_fp)) != EOF)
        {
            // Little endian word construction
            word = (word >> 8) | ((uint32_t)byte <<24);

            // When read four bytes, write the word to the AxiMemory component
            if (bytecount[fidx] % 4 == 3)
            {
                cosim.transWrite(address, word);
                address += 4;
                cosim.tick(1);
            }
            bytecount[fidx]++;
        }

        // Flush any partial word
        if (bytecount[fidx]%4)
        {
            word = word >> 8*(4-(bytecount[fidx]%4));
            cosim.transWrite(address, word);
        }

        // Close the image file
        fclose(img_fp);

        cosim.tick(10);

        // Flag initialisation is finished for this file
        mem_init_sync++;
        localsync = mem_init_sync;

        while(mem_init_sync == localsync)
        {
            cosim.tick(1);
        }
    }

    // Flag that this program has finished
    cosim.tick(10, true, error);

    // If ever got this far then sleep forever
    SLEEPFOREVER;
}

