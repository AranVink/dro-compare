#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#pragma pack(1) // Ensure no padding between struct members

typedef struct {
    char cSignature[8];
    uint16_t iVersionMajor;
    uint16_t iVersionMinor;
} DROFileHeader;

typedef struct {
    uint32_t iLengthPairs;
    uint32_t iLengthMS;
    uint8_t iHardwareType;
    uint8_t iFormat;
    uint8_t iCompression;
    uint8_t iShortDelayCode;
    uint8_t iLongDelayCode;
    uint8_t iCodemapLength;
    uint8_t iCodemap[128];
} DROv20Header;

typedef struct {
    uint8_t iRegister;
    uint8_t iValue;
} DRODataPair;

typedef struct {
    uint32_t iLengthPairs;
    DRODataPair *dataPairs;
    uint32_t iTotalDelayLength;
} DRODataArray;

int get_delay(DRODataPair pair, uint8_t short_delay_code, uint8_t long_delay_code) {
    if (pair.iRegister == short_delay_code)
    {
        return pair.iValue + 1;
    }
    if (pair.iRegister == long_delay_code)
    {
        return (pair.iValue + 1) * 256;
    }
    return 0;
}

DRODataArray calculate_data_pairs(const char *file_path, bool ignore_delays) {
    DRODataArray dataArray;
    dataArray.iLengthPairs = 0;
    dataArray.iTotalDelayLength = 0;
    
    dataArray.dataPairs = NULL;

    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file");
        return dataArray; // Return an empty array for error
    }

    // Read the main file header
    DROFileHeader file_header;
    fread(&file_header, sizeof(DROFileHeader), 1, file);

    // Check the signature
    if (memcmp(file_header.cSignature, "DBRAWOPL", 8) != 0) {
        printf("Invalid DRO file. Signature mismatch.\n");
        fclose(file);
        return dataArray; // Return an empty array for error
    }

    if (file_header.iVersionMajor != 2 || file_header.iVersionMinor != 0) {
        printf("Unsupported DRO version.\n");
        fclose(file);
        return dataArray; // Return an empty array for error
    }

    // Read version 2.0 header
    DROv20Header dro_header;
    fread(&dro_header, sizeof(DROv20Header), 1, file);

    // Allocate memory for the array
    dataArray.dataPairs = (DRODataPair *)malloc(dro_header.iLengthPairs * sizeof(DRODataPair));
    if (!dataArray.dataPairs) {
        perror("Memory allocation error");
        fclose(file);
        dataArray.iLengthPairs = 0;
        return dataArray; // Return an empty array for error
    }

    // Read song data value pairs in a loop, excluding short and long delays
    for (uint32_t i = 0; i < dro_header.iLengthPairs; ++i) {
        DRODataPair data_pair;
        fread(&data_pair, sizeof(DRODataPair), 1, file);

        if (!ignore_delays) {
            // Just add everything
            dataArray.dataPairs[dataArray.iLengthPairs++] = data_pair;
        } else  {
            // Exclude short and long delays
            if (data_pair.iRegister != dro_header.iShortDelayCode && data_pair.iRegister != dro_header.iLongDelayCode)
            {
                dataArray.dataPairs[dataArray.iLengthPairs++] = data_pair;
            }
        }
        dataArray.iTotalDelayLength = dataArray.iTotalDelayLength + get_delay(data_pair, dro_header.iShortDelayCode, dro_header.iLongDelayCode);
    }

    fclose(file);
    return dataArray;
}

int compare_dro_files(const char *file_path1, const char *file_path2, bool match_delays, bool ignore_delays) {
    DRODataArray dataArray1 = calculate_data_pairs(file_path1, ignore_delays);
    DRODataArray dataArray2 = calculate_data_pairs(file_path2, ignore_delays);

    if (dataArray1.iLengthPairs == 0 || dataArray2.iLengthPairs == 0) {
        printf("Error calculating data pairs.\n");
        return 1; // Return an error code
    }

    printf("File 1 Pairs: %u\n", dataArray1.iLengthPairs);
    printf("File 2 Pairs: %u\n", dataArray2.iLengthPairs);

    printf("File 1 Delays: %u\n", dataArray1.iTotalDelayLength);
    printf("File 2 Delays: %u\n", dataArray2.iTotalDelayLength);

    // Compare data pairs
    if (dataArray1.iLengthPairs != dataArray2.iLengthPairs) {
        printf("DRO files have different lengths.\n");
        free(dataArray1.dataPairs);
        free(dataArray2.dataPairs);
        return 1; // Return an error code
    }

    for (uint32_t i = 0; i < dataArray1.iLengthPairs; ++i) {
        if (dataArray1.dataPairs[i].iRegister != dataArray2.dataPairs[i].iRegister ||
            dataArray1.dataPairs[i].iValue != dataArray2.dataPairs[i].iValue) {
            printf("DRO files differ at pair %u.\n", i + 1);
            free(dataArray1.dataPairs);
            free(dataArray2.dataPairs);
            return 1; // Return an error code
        }
    }

    if (match_delays && (dataArray1.iTotalDelayLength != dataArray2.iTotalDelayLength)) {
        printf("DRO files have non-matching delays.\n");
        free(dataArray1.dataPairs);
        free(dataArray2.dataPairs);
        return 1; // Return an error code
    }

    printf("DRO files are identical.\n");

    // Free the allocated memory
    free(dataArray1.dataPairs);
    free(dataArray2.dataPairs);

    return 0; // Success
}

int main(int argc, char *argv[]) {
    bool removeDelays = false; // Default is not to remove delays
    bool matchDelays = false; // Default is to not match on length (match all)

    // Parse command-line options
    int opt;
    while ((opt = getopt(argc, argv, "rm")) != -1) {
        switch (opt) {
            case 'r':
                removeDelays = true;
                break;
            case 'm':
                matchDelays = true;
                break;
            default:
                fprintf(stderr, "Usage: %s [-r(remove delays)][-m(match total delays) <dro_file1> <dro_file2>\n", argv[0]);
                return 1; // Return an error code
        }
    }

    // Check if enough arguments are provided
    if (optind + 2 != argc) {
        fprintf(stderr, "Usage: %s [-r(remove delays)][-m(match total delays)] <dro_file1> <dro_file2>\n", argv[0]);
        return 1; // Return an error code
    }

    const char *file_path1 = argv[optind];
    const char *file_path2 = argv[optind + 1];

    int result = compare_dro_files(file_path1, file_path2, matchDelays, removeDelays);
    return result;
}