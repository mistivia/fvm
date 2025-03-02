#include <stdio.h>

#include "as_tokenizer.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: fvm-as <inputfile>\n");
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    TokenStream* ts = makeTokenStream(fp);
    return 0;
}

