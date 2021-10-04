#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

#include "record.h"
#include "coord_query.h"

struct node {
    struct record *rs;
    double point;
    int axis;
    struct node *left;
    struct node *right;
};

int compare_lon(struct record *arg1, struct record *arg2) {
    if (arg1->lon > arg2->lon)
        return 1;
    else {
        return -1;
    }
}

int compare_lat(struct record *arg1, struct record *arg2) {
    if (arg1->lat > arg2->lat)
        return 1;
    else {
        return -1;
    }
}

struct node* construction(struct record *rs, int n, int depth) {
    if (n < 1) {
      return NULL;
    }

    struct node *nd = malloc(sizeof(struct node));
    
    nd->axis = depth % 2;
    if (nd->axis == 0) {
      qsort(rs, n, sizeof(struct record), compare_lon);
    } else {
      qsort(rs, n, sizeof(struct record), compare_lat);
    }
    nd->rs = &rs[(n + 1)/2];

    nd->left = construction(rs, (n + 1)/2 - 1, depth + 1);

    if ((n + 1) % 2 == 0) {
      nd->right = construction(rs + n, (n + 1)/2 - 2, depth + 1);
    } else {
      nd->right = construction(rs + n, (n + 1)/2 - 1, depth + 1);
    }

    return nd;
}

struct node* mk_kdtree(struct record* rs, int n) {
  return construction(rs, n, 0);
}

void free_kdtree(struct node *data) {
  free(data);
}

const struct record* lookup_kdtree(struct node *data, double lon, double lat) {
  // double lon_data = data->rs[0].lon;
  // double lat_data = data->rs[0].lat;
  // double current_smallest_dist = sqrt(pow(lon_data - lon, 2.0) + pow(lat_data - lat, 2.0));
  // const struct record* current_record = data->rs;
  // for (int i = 1; i < data->n - 1; i++) {
  //   lon_data = data->rs[i].lon;
  //   lat_data = data->rs[i].lat;
  //   double current_dist_i = sqrt(pow(lon_data - lon, 2.0) + pow(lat_data - lat, 2.0));
  //   if (current_dist_i < current_smallest_dist) {
  //     current_smallest_dist = current_dist_i;
  //     current_record = data->rs;
  //   }
  // }
  // return current_record;
  assert(1);
}

int main(int argc, char** argv) {
  return coord_query_loop(argc, argv,
                          (mk_index_fn)mk_kdtree,
                          (free_index_fn)free_kdtree,
                          (lookup_fn)lookup_kdtree);
}
