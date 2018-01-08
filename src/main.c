#include "listing_ui.h"
#include "catalog.h"

void main(void) {
    Catalog *catalog;
    newCatalogRandom(10, 1000, &catalog);
    ListingConfig config;

    interactListing(catalog, &config);
}
