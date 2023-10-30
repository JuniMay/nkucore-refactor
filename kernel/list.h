#ifndef KERNEL_LIST_H_
#define KERNEL_LIST_H_

#define offset_of(type, member) ((size_t)(&((type*)0)->member))

#define container_of(ptr, type, member) \
  ((type*)((char*)(ptr)-offset_of(type, member)))

typedef struct list_entry {
  struct list_entry *prev, *next;
} list_entry_t;

static inline void list_init(list_entry_t* elm) {
  elm->prev = elm->next = elm;
}

static inline void list_add(list_entry_t* listelm, list_entry_t* elm) {
  listelm->next->prev = elm;
  elm->next = listelm->next;
  listelm->next = elm;
  elm->prev = listelm;
}

static inline void list_add_after(list_entry_t* listelm, list_entry_t* elm) {
  list_add(listelm, elm);
}

static inline void list_add_before(list_entry_t* listelm, list_entry_t* elm) {
  list_add(listelm->prev, elm);
}

static inline void list_del(list_entry_t* listelm) {
  listelm->prev->next = listelm->next;
  listelm->next->prev = listelm->prev;
}

static inline int list_empty(list_entry_t* list) {
  return list->next == list;
}

#endif  // KERNEL_LIST_H_