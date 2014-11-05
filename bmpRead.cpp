/*
	***** bmpRead.cc *****

Code to read in a BMP image file.

CSC433/533 Computer Graphics - Fall 2014

Author: John M. Weiss, Ph.D.
Written March 23, 2006.

Ref: based on code from RgbImage.cc (Buss, 3-D Computer Graphics, 2003).
*/

// Windoze hacks (JMW 121026)
#if defined(_WIN32)
#  ifndef APIENTRY
#    define APIENTRY
#  endif
#  ifndef WINGDIAPI
#    define WINGDIAPI
#  endif
#endif

#include <GL/gl.h>
#include <stdio.h>

bool LoadBmpFile( const char* filename, int &NumRows, int &NumCols, unsigned char* &ImagePtr );
static short readShort( FILE* infile );
static int readLong( FILE* infile );
static void skipChars( FILE* infile, int numChars );
static inline int GetNumBytesPerRow( int NumCols );

/* ********************************************************************
 *  LoadBmpFile
 *  Read into memory an RGB image from an uncompressed BMP file.
 *  Return true for success, false for failure.
 *  Author: Sam Buss December 2001.
 **********************************************************************/

bool LoadBmpFile( const char* filename, int &NumRows, int &NumCols, unsigned char* &ImagePtr )
{
    FILE* infile = fopen( filename, "rb" );		// Open for reading binary data
    if ( !infile )
    {
        fprintf( stderr, "LoadBmpFile(): unable to open file: %s\n", filename );
        return false;
    }

    bool fileFormatOK = false;
    int bChar = fgetc( infile );
    int mChar = fgetc( infile );
    if ( bChar == 'B' && mChar == 'M' )
    {			// If starts with "BM" for "BitMap"
        skipChars( infile, 4 + 2 + 2 + 4 + 4 );			// Skip 4 fields we don't care about
        NumCols = readLong( infile );
        NumRows = readLong( infile );
        skipChars( infile, 2 );					// Skip one field
        int bitsPerPixel = readShort( infile );
        skipChars( infile, 4 + 4 + 4 + 4 + 4 + 4 );		// Skip 6 more fields

        if ( NumCols > 0 && NumCols <= 100000 && NumRows > 0 && NumRows <= 100000
                && bitsPerPixel == 24 && !feof( infile ) )
        {
            fileFormatOK = true;
        }
    }

    if ( !fileFormatOK )
    {
        fclose ( infile );
        fprintf( stderr, "Not a valid 24-bit bitmap file: %s.\n", filename );
        return false;
    }

    // Allocate memory
    ImagePtr = new unsigned char[ NumRows * GetNumBytesPerRow( NumCols ) ];
    if ( !ImagePtr )
    {
        fclose ( infile );
        fprintf( stderr, "Unable to allocate memory for %i x %i bitmap: %s.\n",
                 NumRows, NumCols, filename );
        return false;
    }

    unsigned char* cPtr = ImagePtr;
    for ( int i = 0; i < NumRows; i++ )
    {
        int j;
        for ( j = 0; j < NumCols; j++ )
        {
            *( cPtr + 2 ) = fgetc( infile );	// Blue color value
            *( cPtr + 1 ) = fgetc( infile );	// Green color value
            *cPtr = fgetc( infile );		// Red color value
            cPtr += 3;
        }
        int k = 3 * NumCols;					// Num bytes already read
        for ( ; k < GetNumBytesPerRow( NumCols ); k++ )
        {
            fgetc( infile );				// Read and ignore padding;
            *( cPtr++ ) = 0;
        }
    }

    if ( feof( infile ) )
    {
        fclose ( infile );
        fprintf( stderr, "Premature end of file: %s.\n", filename );
        return false;
    }

    fclose( infile );	// Close the file

    return true;
}

// Rows are word aligned
static inline int GetNumBytesPerRow( int NumCols )
{
    return ( ( 3 * NumCols + 3 ) >> 2 ) << 2;
}


// read a 16-bit integer from the input file
short readShort( FILE* infile )
{
    unsigned char lowByte, hiByte;
    lowByte = fgetc( infile );			// Read the low order byte (little endian form)
    hiByte = fgetc( infile );			// Read the high order byte

    // Pack together
    short ret = hiByte;
    ret <<= 8;
    ret |= lowByte;
    return ret;
}

// read a 32-bit integer from the input file
int readLong( FILE* infile )
{
    unsigned char byte0, byte1, byte2, byte3;
    byte0 = fgetc( infile );			// Read bytes, low order to high order
    byte1 = fgetc( infile );
    byte2 = fgetc( infile );
    byte3 = fgetc( infile );

    // Pack together
    int ret = byte3;
    ret <<= 8;
    ret |= byte2;
    ret <<= 8;
    ret |= byte1;
    ret <<= 8;
    ret |= byte0;
    return ret;
}

// skip over given number of bytes in input file
void skipChars( FILE* infile, int numChars )
{
    for ( int i = 0; i < numChars; i++ )
    {
        fgetc( infile );
    }
}
