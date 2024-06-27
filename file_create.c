#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__STDC_LIB_EXT1__) && (__STDC_WANT_LIB_EXT1__ == 1)
    // Use C11 Annex K safer snprintf_s
    #define USE_SAFE_SNPRINTF
#endif

void generate_file(const char* filename, const char* content);

int main() {
    const char* filenames[6] = {
        "F1.txt",
        "F2.txt",
        "F3.txt",
        "F4.txt",
        "F5.txt",
        "F6.txt"
    };

    const char* contents[6] = {
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vivamus lacinia odio vitae vestibulum vestibulum. Cras venenatis euismod malesuada. Nulla posuere sollicitudin aliquam. Sed placerat, libero sit amet aliquam dictum, nisi erat varius dolor, nec elementum nunc libero ut magna. Ut sollicitudin nulla sit amet justo auctor, a auctor neque ullamcorper.",
        "The quick brown fox jumps over the lazy dog. This is a pangram, a sentence that uses every letter of the alphabet at least once. Pangrams are useful for testing fonts and keyboards. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog.",
        "In a galaxy far, far away, there was a small planet named Xandar. The inhabitants of Xandar were known for their advanced technology and peaceful nature. They lived in harmony with their environment, using renewable energy and sustainable practices. The Xandarians valued knowledge and education, and their libraries were filled with ancient texts and modern research.",
        "Once upon a time, in a dense forest, there lived a cunning fox and a clever rabbit. They were always trying to outsmart each other. One day, the rabbit found a hidden stash of carrots and devised a plan to keep it secret from the fox. The fox, however, had his own plans to discover the rabbit's secret. Their rivalry led to many adventurous escapades in the forest.",
        "The history of computing dates back to ancient times with the invention of the abacus. Over the centuries, many significant advancements were made, leading to the creation of the first mechanical computers in the 19th century. The 20th century saw the development of electronic computers, which revolutionized many aspects of life. Today, computers are an integral part of society, powering everything from smartphones to space exploration.",
        "In the world of quantum mechanics, particles can exist in multiple states simultaneously, a phenomenon known as superposition. This counterintuitive behavior is a fundamental aspect of quantum computing, which has the potential to solve complex problems much faster than classical computers. Researchers are exploring various approaches to harness the power of quantum mechanics for practical applications."
    };

    for (int i = 0; i < 6; i++) {
        generate_file(filenames[i], contents[i]);
    }

    printf("6 files of size 300 bytes each with unique data created successfully.\n");
    return 0;
}

void generate_file(const char* filename, const char* content) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    // Ensure buffer size is enough to hold the content and a null terminator
    char buffer[301];

#ifdef USE_SAFE_SNPRINTF
    // Use the safer snprintf_s function if available
    snprintf_s(buffer, sizeof(buffer), _TRUNCATE, "%s", content);
#else
    // Use snprintf with boundary checks
    snprintf(buffer, sizeof(buffer), "%.300s", content);
#endif

    // Write data to the file
    size_t bytes_written = fwrite(buffer, sizeof(char), 300, file);
    if (bytes_written != 300) {
        perror("Error writing to file");
        fclose(file);
        exit(1);
    }

    fclose(file);
}
