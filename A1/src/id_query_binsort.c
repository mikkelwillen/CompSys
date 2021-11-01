#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "record.h"
#include "id_query.h"

struct index_record {
    int64_t osm_id;
    const struct record *record;
};

struct indexed_data {
    struct index_record *irs;
    int n;
};

int compare(struct index_record *arg1, struct index_record *arg2) {
    if (arg1->osm_id > arg2->osm_id)
        return 1;
    else {
        return -1;
    }
}

// Inspiration for code for binary search found here https://www.geeksforgeeks.org/binary-search/
const struct record* binarySearch(struct indexed_data *data, int64_t needle, int l, int r) {
    if(r >= l) {
        int mid = l + (r - l) / 2;

        if(data->irs[mid].osm_id == needle) {
            return data->irs[mid].record;
        }

        if(data->irs[mid].osm_id > needle) {
            return binarySearch(data, needle, l, mid - 1);
        }

        return binarySearch(data, needle, mid + 1, r);
    }

    return NULL;
}

struct indexed_data* mk_indexed(struct record* rs, int n) {
    struct indexed_data *id = malloc(sizeof(struct indexed_data));
    id->n = n;
    id->irs = malloc(sizeof(struct index_record) * n);

    for (int i = 0; i < n - 1; i++) {
        id->irs[i].record = &rs[i];
        id->irs[i].osm_id = rs[i].osm_id;
    }

    qsort(&id->irs[0], n, sizeof(struct index_record), compare);
    return id;
}

void free_indexed(struct indexed_data* data) {
    free(data->irs);
    free(data);
}

const struct record* lookup_indexed(struct indexed_data *data, int64_t needle) {
    return binarySearch(data, needle, 0, data->n - 1);
}

int main(int argc, char** argv) {
  return id_query_loop(argc, argv,
                    (mk_index_fn)mk_indexed,
                    (free_index_fn)free_indexed,
                    (lookup_fn)lookup_indexed);
}
