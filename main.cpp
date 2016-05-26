//===================================================
//  Created by: Tyler Sriver                        *
//  Date: March 30, 2016                            *
//  File: main.cpp                                  *
//===================================================
#include <iostream>
#include <fstream>
using namespace std;

int main() {

    // *** Initialize Variables ***
    const int FRAMES = 256;     // Number of Frames
    const int FRAME_SIZE = 256; // Size of each Frame
    const int TLB_SIZE = 16;    // Length of TLB

    int temp;                   // Variable for reading in num
    int pageNum;                // The page Number
    int offset;                 // The Offset
    int value;                  // Value at memory location
    int physAddress;            // Physical Address

    int TLB[TLB_SIZE][2] = {0}; // TLB Table
    int pageTable[FRAMES][2];   // Page Table
    char buff[FRAME_SIZE];      // Read in from backing store buffer
    unsigned int mem[FRAMES][FRAME_SIZE];
    bool inTLB = false;         // Flag for TLB HIT
    bool inPageTable = false;   // Flag for PAge table hit

    int frameNum;               // Frame Number
    int tlbHit = 0;             // Number of successful TLB hits
    int pageFault = 0;          // Nubmer of Page Faults
    int numAddresses = 0;       // Number of Addresses
    int memCount = 0;
    int tableCount = 0;
    double faultPercent;        //Percentage of page faults
	
    // *** Open Logical Addresses File ***
    ifstream file;
    file.open("addresses.txt");

    // *** Open Output File **
    ofstream output("output.txt");


    // *** While Loop For handling Addresses ***
    while(file >> temp) {

        // Reset Flags
        inTLB = false;
        inPageTable = false;
        numAddresses++;

        // Get offset and page number
        offset = temp & 0xFF;
        pageNum = temp & 0xFF00;
        pageNum = pageNum >> 8;


        // Search For Frame in TLB
        for(int i = 0; i < 16; i++){
            if(pageNum == TLB[i][0])
            {
                frameNum = TLB[i][1];
                tlbHit++;
                inTLB = true;
            }
        }

        // Search in Page Table
        if(!inTLB)
        {
            for(int i = 0; i < FRAMES; i++)
            {
                if(pageNum == pageTable[i][0])
                {
                    frameNum = pageTable[i][1];
                    inPageTable = true;
                }
            }
            if(!inPageTable)
            {
                // *** Open Backing Store ***
                ifstream backingStore;
                backingStore.open("BACKING_STORE.bin", ios::binary | ios::in);

                pageFault++;
                // Find the page
                backingStore.seekg((pageNum * FRAME_SIZE), backingStore.beg);
                backingStore.read(buff, FRAME_SIZE);

                // Save page
                for(int i = 0; i < sizeof(buff); i++)
                {
                    mem[memCount][i] = buff[i];
                }
				// Close BACKING_STORE
                backingStore.close();

                //Find Requested Byte
                pageTable[memCount][0] = pageNum;
                pageTable[memCount][1] = memCount;
                frameNum = memCount;
                memCount = (memCount + 1) % FRAMES;
            }

            // Save Page in TLB
            TLB[tableCount][0] = pageNum;
            TLB[tableCount][1] = frameNum;
            tableCount = (tableCount + 1) % TLB_SIZE;
        }

        // Get value and Physical Addresss from memory
        value = mem[frameNum][offset];
        physAddress = (frameNum * FRAME_SIZE) + offset;

        // Output to File
        output << "Virtual Address: " << temp << " Physical address: " << physAddress << " Value: " << value << endl;
    }

    // Record Statistics
    output << "TLB Hits: " << tlbHit << endl;
    output << "Page Faults: " << pageFault << endl;

    faultPercent = (double(pageFault) / double(numAddresses)) * 100;
    output << "Percentage of faults: " << faultPercent << endl;

    // Close Files
    file.close();
    output.close();
    return 0;
}
