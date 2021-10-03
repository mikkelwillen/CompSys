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
    struct node left;
    struct node right;
};

struct node* construction(struct record* rs, int n, int depth) {
    struct node *nd = malloc(sizeof(struct node));
    
}

struct node* mk_kdtree(struct record* rs, int n) {
  
  nd->rs = rs;
  nd->n = n;
  return nd;
}

void free_kdtree(struct naive_data* data) {
  free(data);
}

const struct record* lookup_kdtree(struct naive_data *data, double lon, double lat) {
  double lon_data = data->rs[0].lon;
  double lat_data = data->rs[0].lat;
  double current_smallest_dist = sqrt(pow(lon_data - lon, 2.0) + pow(lat_data - lat, 2.0));
  const struct record* current_record = data->rs;
  for (int i = 1; i < data->n - 1; i++) {
    lon_data = data->rs[i].lon;
    lat_data = data->rs[i].lat;
    double current_dist_i = sqrt(pow(lon_data - lon, 2.0) + pow(lat_data - lat, 2.0));
    if (current_dist_i < current_smallest_dist) {
      current_smallest_dist = current_dist_i;
      current_record = data->rs;
    }
  }
  return current_record;
}

int main(int argc, char** argv) {
  return coord_query_loop(argc, argv,
                          (mk_index_fn)mk_kdtree,
                          (free_index_fn)free_kdtree,
                          (lookup_fn)lookup_kdtree);
}
