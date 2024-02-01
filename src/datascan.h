#ifndef __datascan_h__
#define __datascan_h__

struct importdatameta {
    void *data;
    void (*progress)(void *data, const char *path);
    int (*cancelled)(void *data);
};

enum {
    IMPORTDATA_COPIED = 2,     // A file was imported.
    IMPORTDATA_SKIPPED = 1,    // Identified, but passed over.
    IMPORTDATA_OK = 0,         // Nothing good nor bad.
    IMPORTDATA_ERROR = -1,
};

int ImportDataFromPath(const char *path, struct importdatameta *cbs);

#endif
