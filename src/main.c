#include "listing_ui.h"
#include "catalog.h"

void main(void) {
    Catalog *catalog;
    newCatalogRandom(&catalog, 10, 1000);
    freeCatalog(catalog);

    interactListing(catalog, NULL_CONFIG());
}
