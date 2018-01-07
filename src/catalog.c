#include "catalog.h"
#include "algorithms/dictionary.h"
#include "algorithms/leaderboard.h"
#include "algorithms/ordered_list.h"

typedef struct CategoryRecord {
    Category * category;
    Price totalValue;
    size_t totalProducts;
    size_t totalInstances;
} CategoryRecord;

typedef struct Catalog {
    OrderedList * categories;
    Dictionary * byId;
    OrderedList * byName;
    OrderedList * byPrice;
} Catalog;
