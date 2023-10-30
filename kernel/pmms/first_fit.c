#include "first_fit.h"

static void first_fit_init() {
  list_init(&free_area.head);
  free_area.num_free_pages = 0;
}

static void first_fit_init_memmap(page_t* base, size_t n) {
  page_t* page = base;
  for (; page != base + n; page++) {
    page->flags = 0;
    page->num_pages = 0;
    page->ref = 0;
  }

  base->num_pages = n;
  page_set_allocable(base);
  free_area.num_free_pages += n;

  // Assume list empty.
  list_add(&free_area.head, &base->link);
}

static page_t* first_fit_alloc_pages(size_t n) {
  if (n > free_area.num_free_pages) {
    return NULL;
  }

  page_t* page = NULL;

  list_entry_t* entry = &free_area.head;
  while ((entry = entry->next) != &free_area.head) {
    page_t* p = container_of(entry, page_t, link);
    if (p->num_pages >= n) {
      page = p;
      break;
    }
  }

  if (page == NULL) {
    return NULL;
  }

  if (page->num_pages > n) {
    page_t* new_page = page + n;
    new_page->num_pages = page->num_pages - n;
    page_set_allocable(new_page);
    list_add_after(&page->link, &new_page->link);
  }

  page->num_pages = n;
  page_clear_allocable(page);
  free_area.num_free_pages -= n;
  list_del(&page->link);

  return page;
}

static void first_fit_free_pages(page_t* base, size_t n) {
  // TODO
}

static size_t first_fit_num_free_pages() {
  return free_area.num_free_pages;
}

pmm_t pmm_first_fit = {
  .name = "first_fit",
  .init = first_fit_init,
  .init_memmap = first_fit_init_memmap,
  .alloc_pages = first_fit_alloc_pages,
  .free_pages = first_fit_free_pages,
  .num_free_pages = first_fit_num_free_pages,
};
