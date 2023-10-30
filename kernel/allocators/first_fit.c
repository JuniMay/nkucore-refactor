#include "allocators/first_fit.h"

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
  page_set_flags(base, PG_ALLOCABLE);
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
    page_set_flags(new_page, PG_ALLOCABLE);
    list_add_after(&page->link, &new_page->link);
  }

  page->num_pages = n;
  page_clear_flags(page, PG_ALLOCABLE);
  free_area.num_free_pages -= n;
  list_del(&page->link);

  return page;
}

static void first_fit_free_pages(page_t* base, size_t n) {
  page_t* page = base;
  for (; page != base + n; page++) {
    page->flags = 0;
    page->ref = 0;
  }

  base->num_pages = n;
  page_set_flags(base, PG_ALLOCABLE);

  free_area.num_free_pages += n;

  if (list_empty(&free_area.head)) {
    list_add_after(&free_area.head, &(base->link));
  } else {
    list_entry_t* entry = &free_area.head;
    while ((entry = entry->next) != &free_area.head) {
      page_t* page = container_of(entry, page_t, link);
      if (base < page) {
        list_add_before(entry, &(base->link));
        break;
      } else if (entry->next == &free_area.head) {
        list_add_after(entry, &(base->link));
        break;
      }
    }
  }

  // try to combine
  list_entry_t* entry = base->link.prev;
  if (entry != &free_area.head) {
    page_t* page = container_of(entry, page_t, link);
    if (page + page->num_pages == base) {
      page->num_pages += base->num_pages;
      page_clear_flags(base, PG_ALLOCABLE);
      list_del(&(base->link));
      base = page;
    }
  }

  entry = base->link.next;
  if (entry != &free_area.head) {
    page_t* page = container_of(entry, page_t, link);
    if (base + base->num_pages == page) {
      base->num_pages += page->num_pages;
      page_clear_flags(page, PG_ALLOCABLE);
      list_del(&(page->link));
    }
  }
}

static size_t first_fit_num_free_pages() {
  return free_area.num_free_pages;
}

page_allocator_t first_fit_page_allocator = {
  .name = "first_fit",
  .init = first_fit_init,
  .init_memmap = first_fit_init_memmap,
  .alloc_pages = first_fit_alloc_pages,
  .free_pages = first_fit_free_pages,
  .num_free_pages = first_fit_num_free_pages,
};
