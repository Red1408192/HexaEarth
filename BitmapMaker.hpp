#include <cstdint> // for specific size integers
#include <fstream> // for file handling
using namespace std;

struct BmpHeader {
    char bitmapSignatureBytes[2] = {'B', 'M'};
    uint32_t sizeOfBitmapFile = 54 + 786432;
    uint16_t reservedBytes1 = 0;
    uint16_t reservedBytes2 = 0;
    uint32_t pixelDataOffset = 54;
};

struct BmpInfoHeader {
    uint32_t sizeOfThisHeader = 40;
    int32_t width = 512; // in pixels
    int32_t height = 512; // in pixels
    uint16_t numberOfColorPlanes = 1; // must be 1
    uint16_t colorDepth = 24;
    uint32_t compressionMethod = 0;
    uint32_t rawBitmapDataSize = 0; // generally ignored
    int32_t horizontalResolution = 2835; // in pixel per meter
    int32_t verticalResolution = 2835; // in pixel per meter
    uint32_t colorTableEntries = 0;
    uint32_t importantColors = 0;
};

struct Pixel {
uint8_t blue = 0;
uint8_t green = 0;
uint8_t red = 0;
};

class OutputBitmap{
    ofstream image = ofstream("output.bmp", ios::binary);
    public:
        void beginMap(int size){
            uint32_t pixelSize = size*size*24;
            BmpHeader header = {{'B', 'M'}, (uint32_t)54 + pixelSize, 0,0, 54};
            BmpInfoHeader infoHeader = {40, size, size, 1, 24, 0, pixelSize, 2835, 2835, 0, 0};

            image.write((char *) &header, 14);
            image.write((char *) &infoHeader, 40);
        }

        void InsertMap(float t, float q, float p){
            int tg = 127+(127*t);
            int qg = 127+(127*q);
            int pg = 127+(127*p);
            Pixel pixel = {(uint8_t)tg, (uint8_t)qg, (uint8_t)pg};
            image.write((char *) &pixel, 3);
        }

        void CloseMap(){
            image.close();
        }
};
