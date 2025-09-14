#include "compat.h"
#include "baselayer.h"
#include "cache1d.h"
#include "build.h"
#include "datascan.h"

enum {
    COPYFILE_OK = 0,
    COPYFILE_ERR_EXISTS = -1,
    COPYFILE_ERR_OPEN = -2,
    COPYFILE_ERR_RW = -3,
    COPYFILE_ERR_CANCELLED = -4,
};


// Copy the contents of 'fh' to file 'fname', but only if 'fname' doesn't already exist.
static int CopyFile(int fh, int size, const char *fname, struct importdatameta *cbs)
{
    int ofh, rv = COPYFILE_OK;
    ssize_t b=0;
    char buf[16384];

    ofh = open(fname, O_WRONLY|O_BINARY|O_CREAT|O_EXCL, BS_IREAD|BS_IWRITE);
    if (ofh < 0) {
        if (errno == EEXIST) return COPYFILE_ERR_EXISTS;
        return COPYFILE_ERR_OPEN;
    }
    lseek(fh, 0, SEEK_SET);
    do {
        if (cbs->cancelled(cbs->data)) {
            rv = COPYFILE_ERR_CANCELLED;
        } else if ((b = read(fh, buf, sizeof(buf))) < 0) {
            rv = COPYFILE_ERR_RW;
        } else if (b > 0 && write(ofh, buf, b) != b) {
            rv = COPYFILE_ERR_RW;
        }
    } while (b == sizeof(buf) && rv == COPYFILE_OK);
    close(ofh);
    if (size != lseek(fh, 0, SEEK_CUR)) rv = COPYFILE_ERR_RW;  // File not the expected length.
    if (rv != COPYFILE_OK) remove(fname);
    return rv;
}

static int ImportDataFromFile(const char *path, int size, struct importdatameta *cbs)
{
    char *fn;
    int fh, rv;

    fn = strrchr(path, '/');
#ifdef _WIN32
    fn = max(fn, strrchr(path, '\\'));
#endif
    if (!fn) fn = strdup(path);
    else fn = strdup(fn+1);
    strlwr(fn);

    fh = open(path, O_RDONLY|O_BINARY, S_IREAD);
    if (fh < 0) return IMPORTDATA_OK;  // Maybe no permission. Whatever.

    switch (CopyFile(fh, size, fn, cbs)) {
        case COPYFILE_ERR_CANCELLED:
            rv = IMPORTDATA_OK;
            break;
        case COPYFILE_ERR_EXISTS:
            rv = IMPORTDATA_SKIPPED;
            break;
        case COPYFILE_OK: // Copied.
            rv = IMPORTDATA_COPIED;
            break;
        default:
            rv = IMPORTDATA_ERROR;
            break;
    }
    close(fh);
    free(fn);
    return rv;
}

static int ImportFilesFromDir(const char *path, struct importdatameta *cbs)
{
    BDIR *dir;
    struct Bdirent *dirent;
    int found = 0, errors = 0;
    size_t subpathlen;
    char *subpath;
    static const char *pats[] = {
        "tiles???.art",
        "level*.map",
        "palette.dat",
        "lookup.dat",
        "joesnd",
        "songs",
        "intro.smk",
        NULL
    };

    cbs->progress(cbs->data, path);

    dir = Bopendir(path);
    if (!dir) return IMPORTDATA_OK;    // Maybe no permission.

    while ((dirent = Breaddir(dir))) {
        if (cbs->cancelled(cbs->data)) break;

        if (dirent->name[0] == '.' &&
            (dirent->name[1] == 0 || (dirent->name[1] == '.' && dirent->name[2] == 0)))
                continue;
        if (!(dirent->mode & (BS_IFDIR|BS_IFREG))) continue; // Ignore non-directories and non-regular files.
        if (dirent->mode & BS_IFREG) {
            int i;
            for (i=0; pats[i]; i++) {
                if (Bwildmatch(dirent->name, pats[i])) break;
            }
            if (!pats[i]) continue;    // No match to any pattern.
        }

        subpathlen = strlen(path) + 1 + dirent->namlen + 1;
        subpath = malloc(subpathlen);
        if (!subpath) { Bclosedir(dir); break; }
        snprintf(subpath, subpathlen, "%s/%s", path, dirent->name);

        if (dirent->mode & BS_IFDIR) {
            switch (ImportFilesFromDir(subpath, cbs)) {
                case IMPORTDATA_COPIED: found = 1; break;
                case IMPORTDATA_ERROR: errors = 1; break;
            }
        }
        else {
            switch (ImportDataFromFile(subpath, (int)dirent->size, cbs)) {
                case IMPORTDATA_SKIPPED: buildprintf("Skipped %s\n", subpath); break;
                case IMPORTDATA_COPIED: buildprintf("Imported %s\n", subpath); found = 1; break;
                case IMPORTDATA_ERROR: buildprintf("Error importing %s\n", subpath); errors = 1; break;
            }
        }
        free(subpath);
    }
    Bclosedir(dir);
    if (found) return IMPORTDATA_COPIED;   // Finding anything is considered fine.
    else if (errors) return IMPORTDATA_ERROR; // Finding nothing but errors reports back errors.
    return IMPORTDATA_OK;
}

int ImportDataFromPath(const char *path, struct importdatameta *cbs)
{
    struct stat st;
    int found = 0, errors = 0;

    if (stat(path, &st) < 0) return IMPORTDATA_OK;

    if (st.st_mode & S_IFDIR) {
        switch (ImportFilesFromDir(path, cbs)) {
            case IMPORTDATA_COPIED: found = 1; break;
            case IMPORTDATA_ERROR: errors = 1; break;
        }
    }

    if (found) return IMPORTDATA_COPIED;         // Finding anything is considered fine.
    else if (errors) return IMPORTDATA_ERROR; // Finding nothing but errors reports back errors.
    return IMPORTDATA_OK;
}
