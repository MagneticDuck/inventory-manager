#include "listing_ui.h"
#include "catalog.h"

void main(void) {
    Catalog *catalog;
    newCatalogDemo(&catalog);
    ListingConfig config;

    interactListing(catalog, &config);
}
