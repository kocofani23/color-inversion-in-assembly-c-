#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>

#define pixel_max(a) ((a) <= 255 ? (a) : 255)
#define pixel_min(a) ((a) >= 0 ? (a) : 0)

// Function to read an image in grayscale
unsigned char* readImage(const char* filename, int& width, int& height, int& channels) {
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 1); // Load as grayscale
    if (!image) {
        std::cerr << "Failed to load image: " << stbi_failure_reason() << std::endl;
        return nullptr;
    }
    std::cout << "Image loaded successfully!" << std::endl;
    std::cout << "Width: " << width << ", Height: " << height << ", Channels: " << channels << std::endl;
    return image;
}

// Function to write an image to a PNG file
bool writeImage(const char* filename, unsigned char* image, int width, int height) {
    if (!image) {
        std::cerr << "Image data is null before writing!" << std::endl;
        return false;
    }
    if (width <= 0 || height <= 0) {
        std::cerr << "Invalid image dimensions: width = " << width << ", height = " << height << std::endl;
        return false;
    }
    // For grayscale images, stride is the same as the width
    int stride = width;
    if (stbi_write_png(filename, width, height, 1, image, stride) == 0) {
        std::cerr << "Failed to write the image to file: " << filename << std::endl;
        return false;
    }
    std::cout << "Image written successfully to: " << filename << std::endl;
    return true;
}

int main() {
    // Input and output file paths
    const char* inputFilename = "input_image4.png";
    const char* outputFilename1 = "output_image1.png";
    const char* outputFilename2 = "output_image2.png";

    // Image data variables
    int width, height, channels; // channels = 1 (grayscale)
    unsigned int number_of_pixels;

    // Read the input image
    unsigned char* image = readImage(inputFilename, width, height, channels);
    if (!image) 
        return -1; // Exit if the image failed to load

    // Allocate memory for the output image
    unsigned char* outputImage = new unsigned char[width * height];
    if (!outputImage) {
        std::cerr << "Failed to allocate memory for output image!" << std::endl;
        stbi_image_free(image);
        return -1;
    }

    // image is 1d array 
    // with length = width * height
    // pixels can be used as image[i] 
    // pixels can be updated as image[i] = 100, etc.
    // a pixel is defined as unsigned char
    // so a pixel can be 255 at max, and 0 at min.

    /* -------------------------------------------------------- QUESTION-1 -------------------------------------------------------- */
    
    /* Q-1 Inverse the colors of image. 
    Inverse -> pixel_color = 255 - pixel_color */

    number_of_pixels = width * height;
    for (int i = 0; i < number_of_pixels; i++) {
        pixel_max(image[i]);
        pixel_min(image[i]);
    }
        
    __asm {
        XOR ESI, ESI // zero out esi
        XOR EDI, EDI // zero out edi
        XOR EAX, EAX // zero out eax

        MOV ESI, image // move the address(first element) of input image array to esi
        MOV EDI, outputImage  // move the address(first element) of output image array to edi

        MOV ECX, number_of_pixels // start the counter to number of pixels
        MAIN_LOOP:
        MOV EAX, 255 // start eax to 255
        SUB EAX, [ESI] // subtract the value of input image array from 255
        MOV [EDI], EAX // move the subtraction result to the output image array
        INC EDI // increase edi by 1 since unsigned char
        INC ESI // increase esi by 1 since unsigned char
        LOOP MAIN_LOOP
    }

    // Write the modified image as output_image1.png
    if (!writeImage(outputFilename1, outputImage, width, height)) {
        stbi_image_free(image);
        return -1;
    }
    stbi_image_free(outputImage); // Clear the outputImage.

    /* -------------------------------------------------------- QUESTION-2 -------------------------------------------------------- */
    /* Histogram Equalization */

    outputImage = new unsigned char[width * height];
    if (!outputImage) {
        std::cerr << "Failed to allocate memory for output image!" << std::endl;
        stbi_image_free(image);
        return -1;
    }

    unsigned int* hist = (unsigned int*)malloc(sizeof(unsigned int) * 256);
    unsigned int* cdf = (unsigned int*)malloc(sizeof(unsigned int) * 256);

    // Check if memory allocation succeeded
    if (hist == NULL) {
        std::cerr << "Memory allocation for hist failed!" << std::endl;
        return -1;
    }
    if (cdf == NULL) {
        std::cerr << "Memory allocation for cdf failed!" << std::endl;
        free(hist);
        return -1;
    }

    // Both hist and cdf are initialized as zeros.
    for (int i = 0; i < 256; i++) {
        hist[i] = 0;
        cdf[i] = 0;
    }

    // You can define new variables here... As a hint some variables are already defined.
    unsigned int min_cdf, range;
    number_of_pixels = width * height;

    // Q-2 (a) - Compute the histogram of the input image.
    __asm {
        XOR EBX, EBX // zero out ebx
        XOR ESI, ESI // zero out esi 
        XOR EDI, EDI // zero out edi
        MOV ECX, number_of_pixels // initialize the counter to number of pixels

        MOV EBX, image // move the address(first element) of input image array to ebx
        MOV ESI, hist // move the address(first element) if histogram array to esi
        CREATE_HISTOGRAM:
        MOVZX EDI, [EBX] // move to edi the value of input image array
        INC DWORD PTR [ESI + EDI*4]
        INC EBX // increment the index of input image array
        LOOP CREATE_HISTOGRAM
    }
    /* Q-2 (b) - Compute the Cumulative Distribution Function cdf
                    and save it to cdf array which is defined above. */

    // CDF Calculation (cdf[i] = cdf[i-1] + hist[i])
    
    __asm {
        XOR EDI, EDI // zero out edi
        XOR ESI, ESI // zero out esi
        XOR EAX, EAX; // zero out eax

        MOV EDI, hist // move the address(first element) of hist array to edi
        MOV ESI, cdf // move the address(first element) of cdf array to esi

        MOV EAX, [EDI] // move the first element from hist array to eax
        MOV[ESI], EAX // move the element in eax to cdf array

        ADD EDI, 4 // increment hist array index by 4
        ADD ESI, 4 // increment cdf array index by 4

        MOV ECX, 255 // initialize loop to 255
        CREATE_CDF:
        MOV EDX, [EDI] // move the i-th element of hist array to edx
        MOV EAX, [ESI - 4]
        ADD EAX, EDX // add the (i-1)-th element of hist array to eax
        MOV[ESI], EAX // store it in cdf array
        ADD EDI, 4 // increase by 4 index of hist array
        ADD ESI, 4 // increase by 4 index of cdf array
        LOOP CREATE_CDF //loop again
    }

    /* Q-2 (c) - Normalize the Cumulative Distribution Funtion 
                    that you have just calculated on the same cdf array. */

    // Normalized cdf[i] = ((cdf[i] - min_cdf) * 255) / range
    // range = (number_of_pixels - min_cdf)
    min_cdf = cdf[0];
    for (int i = 1; i < 256; i++) {
        if (cdf[i] < min_cdf)
            min_cdf = cdf[i];
    }
    range = number_of_pixels - min_cdf;
    unsigned int constant = 255;

    __asm {
        // Your assembly code here...
        XOR EBX, EBX // zero out ebx
        XOR EAX, EAX // zero out eax
        XOR ECX, ECX // zero out ecx

        MOV EBX, cdf // move the address(first element) of cdf array to ebx
        MOV CX, 256 // initialize the counter cx to 256
        NORMALIZE_CDF:
        MOV EAX, [EBX] // move the element of cdf array to eax
        SUB EAX, min_cdf // subtract min_cdf from eax
        MUL constant // multiply by 255
        DIV range // divide by the range 
        MOV [EBX], EAX // move the new calculated value eax to cdf array
        ADD EBX, 4  // increase the index of cdf array by 4
        LOOP NORMALIZE_CDF
        
    }

    /* Q-2 (d) - Apply the histogram equalization on the image.
                    Write the new pixels to outputImage. */
	// Here you only need to get a pixel from image, say the value of pixel is 107
	// Then you need to find the corresponding cdf value for that pixel
	// The output for the pixel 107 will be cdf[107]
	// Do this for all the pixels in input image and write on output image.
    __asm {
        XOR ESI, ESI // zero out esi
        XOR EDI, EDI // zero out edi
        XOR EBX, EBX // zero out ebx
        XOR EAX, EAX //zero out eax
        XOR ECX, ECX // zero out ecx

        MOV ESI, image // move the address(first element) of image input array to esi
        MOV EDI, cdf // move the address(first element) of cdf array to edi
        MOV EBX, outputImage // move the address(first element) of outputImage array to ebx
        MOV ECX, number_of_pixels // initialize the loop counter to number of pixels

        CREATE_OUTPUT_IMAGE :
        MOVZX EAX, [ESI] // load the pixel value from image array into eax(0 - 255 range)
        MOV EDX, [EDI + EAX*4] // fetch cdf[pixel_value] into edx(each element is 4 bytes)
        MOV[EBX], EDX // store the lower 8 bits of cdf value into outputImage
        INC ESI // move to the next pixel in the image array
        INC EBX // move to the next pixel in the outputImage array
        LOOP CREATE_OUTPUT_IMAGE // decrement ecx and repeat until zero
    }

    // Write the modified image
    if (!writeImage(outputFilename2, outputImage, width, height)) {
        stbi_image_free(image); 
        return -1;
    }

    // Free the image memory
    stbi_image_free(image);
    stbi_image_free(outputImage);

    return 0;
}
