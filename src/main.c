#include "listing_ui.h"
#include "catalog.h"

void main(void) {
    Catalog * catalog;
    newCatalogRandom(&catalog, 10, 1000);

    ListingConfig config;
    config.categoryFilter = 0;
    config.orderAlphabetical = true;
    config.useFilter = true;

    interactListing(catalog, &config);
    freeCatalog(catalog);
}
