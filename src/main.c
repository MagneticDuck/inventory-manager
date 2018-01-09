#include "listing_ui.h"
#include "catalog.h"

void main(void) {
    Catalog *catalog;
    newCatalogRandom(&catalog, 10, 1000);
    freeCatalog(catalog);

    ListingConfig config;
    config.categoryFilter = 0;
    config.useFilter = false;
    config.orderAlphabetical = true;

    interactListing(catalog, &config);
}
