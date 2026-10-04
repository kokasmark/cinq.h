/*

  oooooooo8 ooooo oooo   oooo  ooooooo
o888     88  888   8888o  88 o888   888o
888          888   88 888o88 888     888
888o     oo  888   88   8888 888o  8o888
 888oooo88  o888o o88o    88   88ooo88
                                    88o8
           C Integrated Query         888


Single header library to perform Linq (Language-Integrated Query) operations on Dynamic Arrays.

Copyright (c) 2026 Kokas Márk

*/

#ifndef CINQ_H
#define CINQ_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>


/* Dynamic Array */

#define cinq__meta(ptr)        ((size_t *)(ptr) - 4)
#define cinq_capacity(ptr)     (cinq__meta(ptr)[0])
#define cinq_length(ptr)       (cinq__meta(ptr)[1])
#define cinq__flags(ptr)       (cinq__meta(ptr)[2])
#define cinq__safe_len(ptr)    ((ptr) ? cinq_length(ptr) : (size_t)0)
#define CINQ__TMP 1u

static inline void *cinq_new(size_t elem_size) {
    size_t initial_cap = 4;
    size_t *meta = malloc(4 * sizeof(size_t) + elem_size * initial_cap);
    if (!meta) return NULL;
    meta[0] = initial_cap; // capacity
    meta[1] = 0;           // length
    meta[2] = 0;
    meta[3] = 0;
    return meta + 4;
}

static inline void cinq__release(void *list) {
    if (list) free(cinq__meta(list));
}

#define cinq_free(list) do {                                                                            \
    cinq__release(list);                                                                                \
    (list) = NULL;                                                                                      \
} while (0)

static inline void *cinq_reserve(void *list, size_t elem_size, size_t n) {
    if (!list) return NULL;

    size_t *old_meta = cinq__meta(list);
    size_t new_cap = old_meta[0];
    if (n <= new_cap) return list;

    while (new_cap < n) new_cap *= 2;

    size_t *new_meta = realloc(
        old_meta,
        4 * sizeof(size_t) + new_cap * elem_size
    );
    if (!new_meta) {
        cinq__release(list);
        return NULL;
    }

    new_meta[0] = new_cap;
    return new_meta + 4;
}

static inline void *cinq_grow(void *list, size_t elem_size) {
    return cinq_reserve(list, elem_size, cinq_capacity(list) * 2);
}

#define cinq_to_list(array, length) ({                                                                  \
    __auto_type cinq__tl_a = (array);                                                                   \
    size_t cinq__tl_n = (length);                                                                       \
    __typeof__(cinq__tl_a) cinq__tl_out = NULL;                                                         \
    if (cinq__tl_a && cinq__tl_n > 0) {                                                                 \
        cinq__tl_out = cinq_reserve(cinq_new(sizeof(*cinq__tl_a)),                                      \
                                    sizeof(*cinq__tl_a), cinq__tl_n);                                   \
        if (cinq__tl_out) {                                                                             \
            memcpy(cinq__tl_out, cinq__tl_a,                                                            \
                   cinq__tl_n * sizeof(*cinq__tl_a));                                                   \
            cinq_length(cinq__tl_out) = cinq__tl_n;                                                     \
        }                                                                                               \
    }                                                                                                   \
    cinq__tl_out;                                                                                       \
})

#define cinq_append(list, item) do {                                                                    \
    if ((list) == NULL) {                                                                               \
        (list) = cinq_new(sizeof(*(list)));                                                             \
    }                                                                                                   \
    if ((list) != NULL) {                                                                               \
        if (cinq_length(list) == cinq_capacity(list)) {                                                 \
            (list) = cinq_grow(list, sizeof(*(list)));                                                  \
        }                                                                                               \
        if ((list) != NULL) {                                                                           \
            (list)[ cinq_length(list)++ ] = (item);                                                     \
        }                                                                                               \
    }                                                                                                   \
} while (0)

#define cinq_tmp(list) ({                                                                               \
    __auto_type cinq__tm = (list);                                                                      \
    if (cinq__tm) cinq__flags(cinq__tm) |= CINQ__TMP;                                                   \
    cinq__tm;                                                                                           \
})

#define cinq__consume(c) do {                                                                           \
    if ((c) && (cinq__flags(c) & CINQ__TMP)) cinq_free(c);                                              \
} while (0)

/* ----------------------------------------------------------------- */

static inline int cinq__eq_mem(const void *a, const void *b, size_t n) {
    return memcmp(a, b, n) == 0;
}

static inline int cinq__eq_str(const void *a, const void *b, size_t n) {
    (void)n;
    const char *x = *(const char *const *)a;
    const char *y = *(const char *const *)b;
    if (x == y) return 1;
    if (!x || !y) return 0;
    return strcmp(x, y) == 0;
}

static inline int cinq__eq_float(const void *a, const void *b, size_t n) {
    (void)n;
    return *(const float *)a == *(const float *)b;
}

static inline int cinq__eq_double(const void *a, const void *b, size_t n) {
    (void)n;
    return *(const double *)a == *(const double *)b;
}

#define cinq__eq_for(v) _Generic((v),                                                                   \
    char *:       cinq__eq_str,                                                                         \
    const char *: cinq__eq_str,                                                                         \
    float:        cinq__eq_float,                                                                       \
    double:       cinq__eq_double,                                                                      \
    default:      cinq__eq_mem)

/* ----------------------------------------------------------------- */
/* Operations */

#define cinq__cat(a, b)  cinq__cat_(a, b)
#define cinq__cat_(a, b) a##b

#define cinq__n(...) cinq__n_(0, ##__VA_ARGS__, 8,7,6,5,4,3,2,1,0)
#define cinq__n_(_0,_1,_2,_3,_4,_5,_6,_7,_8,N,...) N

#define cinq__ap(prev, stage)   cinq__ap2(prev, cinq__x_##stage)
#define cinq__ap2(prev, ...)    cinq__ap3(prev, __VA_ARGS__)
#define cinq__ap3(prev, fn, ...) fn(prev, ##__VA_ARGS__)

/* Select */

#define cinq_select_4(list, item_, index_, body) ({                                                     \
    __auto_type cinq__sel_c = (list);                                                                   \
    __typeof__(cinq__sel_c) cinq__sel_out = NULL;                                                       \
    if(cinq__sel_c) {                                                                                   \
        for(size_t index_ = 0; index_ < cinq_length(cinq__sel_c); index_++){                            \
            __auto_type item_ = cinq__sel_c[index_];                                                    \
            cinq_append(cinq__sel_out, (body));                                                         \
        }                                                                                               \
    }                                                                                                   \
    cinq__consume(cinq__sel_c);                                                                         \
    cinq__sel_out;                                                                                      \
})

#define cinq_select_3(list, item_, body) ({                                                             \
    __auto_type cinq__sel_c = (list);                                                                   \
    __typeof__(cinq__sel_c) cinq__sel_out = NULL;                                                       \
    if(cinq__sel_c) {                                                                                   \
        for(size_t cinq__sel_i = 0; cinq__sel_i < cinq_length(cinq__sel_c); cinq__sel_i++){             \
            __auto_type item_ = cinq__sel_c[cinq__sel_i];                                               \
            cinq_append(cinq__sel_out, (body));                                                         \
        }                                                                                               \
    }                                                                                                   \
    cinq__consume(cinq__sel_c);                                                                         \
    cinq__sel_out;                                                                                      \
})

#define cinq_select(...) cinq__cat(cinq__d_select_, cinq__n(__VA_ARGS__))(__VA_ARGS__)
#define cinq__d_select_2(...) cinq__s_select(__VA_ARGS__)
#define cinq__d_select_3(...) cinq_select_3(__VA_ARGS__)
#define cinq__d_select_4(...) cinq_select_4(__VA_ARGS__)
#define cinq__x_cinq__s_select(...) cinq_select_3, ##__VA_ARGS__

/* Each */

#define cinq_each_4(list, item_, index_, body) ({                                                       \
    __auto_type cinq__ea_c = (list);                                                                    \
    if(cinq__ea_c) {                                                                                    \
        for(size_t index_ = 0; index_ < cinq_length(cinq__ea_c); index_++){                             \
            __auto_type item_ = cinq__ea_c[index_];                                                     \
            body;                                                                                       \
        }                                                                                               \
    }                                                                                                   \
    cinq__consume(cinq__ea_c);                                                                          \
})

#define cinq_each_3(list, item_, body) ({                                                               \
    __auto_type cinq__ea_c = (list);                                                                    \
    if(cinq__ea_c) {                                                                                    \
        for(size_t cinq__ea_i = 0; cinq__ea_i < cinq_length(cinq__ea_c); cinq__ea_i++){                 \
            __auto_type item_ = cinq__ea_c[cinq__ea_i];                                                 \
            body;                                                                                       \
        }                                                                                               \
    }                                                                                                   \
    cinq__consume(cinq__ea_c);                                                                          \
})

#define cinq_each(...) cinq__cat(cinq__d_each_, cinq__n(__VA_ARGS__))(__VA_ARGS__)
#define cinq__d_each_2(...) cinq__s_each(__VA_ARGS__)
#define cinq__d_each_3(...) cinq_each_3(__VA_ARGS__)
#define cinq__d_each_4(...) cinq_each_4(__VA_ARGS__)
#define cinq__x_cinq__s_each(...) cinq_each_3, ##__VA_ARGS__

/* Where */

#define cinq_where_4(list, item_, index_, body) ({                                                      \
    __auto_type cinq__wh_c = (list);                                                                    \
    __typeof__(cinq__wh_c) cinq__wh_out = NULL;                                                         \
    if(cinq__wh_c) {                                                                                    \
        for(size_t index_ = 0; index_ < cinq_length(cinq__wh_c); index_++){                             \
            __auto_type item_ = cinq__wh_c[index_];                                                     \
            if((body)){                                                                                 \
                cinq_append(cinq__wh_out, item_);                                                       \
            }                                                                                           \
        }                                                                                               \
    }                                                                                                   \
    cinq__consume(cinq__wh_c);                                                                          \
    cinq__wh_out;                                                                                       \
})

#define cinq_where_3(list, item_, body) ({                                                              \
    __auto_type cinq__wh_c = (list);                                                                    \
    __typeof__(cinq__wh_c) cinq__wh_out = NULL;                                                         \
    if(cinq__wh_c) {                                                                                    \
        for(size_t cinq__wh_i = 0; cinq__wh_i < cinq_length(cinq__wh_c); cinq__wh_i++){                 \
            __auto_type item_ = cinq__wh_c[cinq__wh_i];                                                 \
            if((body)){                                                                                 \
                cinq_append(cinq__wh_out, item_);                                                       \
            }                                                                                           \
        }                                                                                               \
    }                                                                                                   \
    cinq__consume(cinq__wh_c);                                                                          \
    cinq__wh_out;                                                                                       \
})

#define cinq_where(...) cinq__cat(cinq__d_where_, cinq__n(__VA_ARGS__))(__VA_ARGS__)
#define cinq__d_where_2(...) cinq__s_where(__VA_ARGS__)
#define cinq__d_where_3(...) cinq_where_3(__VA_ARGS__)
#define cinq__d_where_4(...) cinq_where_4(__VA_ARGS__)
#define cinq__x_cinq__s_where(...) cinq_where_3, ##__VA_ARGS__

/* Take */
#define cinq_take_2(list, n) ({                                                                         \
    __auto_type cinq__tk_c = (list);                                                                    \
    size_t cinq__tk_len = cinq__safe_len(cinq__tk_c);                                                   \
    size_t cinq__tk_req = (n);                                                                          \
    size_t cinq__tk_n = cinq__tk_req > cinq__tk_len ? cinq__tk_len : cinq__tk_req;                      \
    __typeof__(cinq__tk_c) cinq__tk_out = NULL;                                                         \
    if(cinq__tk_n > 0) {                                                                                \
        cinq__tk_out = cinq_reserve(cinq_new(sizeof(*cinq__tk_c)),                                      \
                                    sizeof(*cinq__tk_c), cinq__tk_n);                                   \
        if(cinq__tk_out) {                                                                              \
            memcpy(cinq__tk_out, cinq__tk_c, cinq__tk_n * sizeof(*cinq__tk_c));                         \
            cinq_length(cinq__tk_out) = cinq__tk_n;                                                     \
        }                                                                                               \
    }                                                                                                   \
    cinq__consume(cinq__tk_c);                                                                          \
    cinq__tk_out;                                                                                       \
})

#define cinq_take(...) cinq__cat(cinq__d_take_, cinq__n(__VA_ARGS__))(__VA_ARGS__)
#define cinq__d_take_1(...) cinq__s_take(__VA_ARGS__)
#define cinq__d_take_2(...) cinq_take_2(__VA_ARGS__)
#define cinq__x_cinq__s_take(...) cinq_take_2, ##__VA_ARGS__

/* Slice */
#define cinq_slice_3(list, start, end) ({                                                               \
    __auto_type cinq__sl_c = (list);                                                                    \
    __typeof__(cinq__sl_c) cinq__sl_out = NULL;                                                         \
    size_t cinq__sl_len = cinq__safe_len(cinq__sl_c);                                                   \
    size_t cinq__sl_s = (start);                                                                        \
    size_t cinq__sl_e = (end);                                                                          \
                                                                                                        \
    if (cinq__sl_s <= cinq__sl_e && cinq__sl_e <= cinq__sl_len) {                                       \
        size_t cinq__sl_n = cinq__sl_e - cinq__sl_s;                                                    \
        cinq__sl_out = cinq_reserve(cinq_new(sizeof(*cinq__sl_c)),                                      \
                                    sizeof(*cinq__sl_c), cinq__sl_n);                                   \
        if (cinq__sl_out) {                                                                             \
            if (cinq__sl_n > 0) {                                                                       \
                memcpy(cinq__sl_out,                                                                    \
                       (char*)cinq__sl_c + cinq__sl_s * sizeof(*cinq__sl_c),                            \
                       cinq__sl_n * sizeof(*cinq__sl_c));                                               \
            }                                                                                           \
            cinq_length(cinq__sl_out) = cinq__sl_n;                                                     \
        }                                                                                               \
    }                                                                                                   \
                                                                                                        \
    cinq__consume(cinq__sl_c);                                                                          \
    cinq__sl_out;                                                                                       \
})

#define cinq_slice(...) cinq__cat(cinq__d_slice_, cinq__n(__VA_ARGS__))(__VA_ARGS__)
#define cinq__d_slice_2(...) cinq__s_slice(__VA_ARGS__)
#define cinq__d_slice_3(...) cinq_slice_3(__VA_ARGS__)
#define cinq__x_cinq__s_slice(...) cinq_slice_3, ##__VA_ARGS__

/* Union */
#define cinq_union_2(listA, listB) ({                                                                   \
    __auto_type cinq__un_a = (listA);                                                                   \
    __auto_type cinq__un_b = (listB);                                                                   \
    __typeof__(cinq__un_a) cinq__un_out = NULL;                                                         \
                                                                                                        \
    if (sizeof(*cinq__un_a) == sizeof(*cinq__un_b)) {                                                   \
        size_t cinq__un_la = cinq__safe_len(cinq__un_a);                                                \
        size_t cinq__un_lb = cinq__safe_len(cinq__un_b);                                                \
        size_t cinq__un_total = cinq__un_la + cinq__un_lb;                                              \
                                                                                                        \
        cinq__un_out = cinq_reserve(cinq_new(sizeof(*cinq__un_a)),                                      \
                                    sizeof(*cinq__un_a), cinq__un_total);                               \
        if (cinq__un_out) {                                                                             \
            if (cinq__un_la > 0) {                                                                      \
                memcpy(cinq__un_out, cinq__un_a, cinq__un_la * sizeof(*cinq__un_a));                    \
            }                                                                                           \
            if (cinq__un_lb > 0) {                                                                      \
                memcpy((char*)cinq__un_out + cinq__un_la * sizeof(*cinq__un_a),                         \
                       cinq__un_b, cinq__un_lb * sizeof(*cinq__un_b));                                  \
            }                                                                                           \
            cinq_length(cinq__un_out) = cinq__un_total;                                                 \
        }                                                                                               \
    }                                                                                                   \
                                                                                                        \
    cinq__consume(cinq__un_a);                                                                          \
    cinq__consume(cinq__un_b);                                                                          \
    cinq__un_out;                                                                                       \
})

#define cinq_union(...) cinq__cat(cinq__d_union_, cinq__n(__VA_ARGS__))(__VA_ARGS__)
#define cinq__d_union_1(...) cinq__s_union(__VA_ARGS__)
#define cinq__d_union_2(...) cinq_union_2(__VA_ARGS__)
#define cinq__x_cinq__s_union(...) cinq_union_2, ##__VA_ARGS__

/* Contains */
#define cinq_contains_2(list, value) ({                                                                 \
    __auto_type cinq__ct_n = (list);                                                                    \
    __typeof__(*cinq__ct_n) cinq__ct_v = (value);                                                       \
    int cinq__ct_found = 0;                                                                             \
    if (cinq__ct_n) {                                                                                   \
        size_t cinq__ct_len = cinq_length(cinq__ct_n);                                                  \
        for (size_t cinq__ct_i = 0; cinq__ct_i < cinq__ct_len; cinq__ct_i++) {                          \
            if (cinq__eq_for(cinq__ct_v)(&cinq__ct_n[cinq__ct_i], &cinq__ct_v, sizeof(cinq__ct_v))) {                                 \
                cinq__ct_found = 1;                                                                     \
                break;                                                                                  \
            }                                                                                           \
        }                                                                                               \
    }                                                                                                   \
    cinq__consume(cinq__ct_n);                                                                          \
    cinq__ct_found;                                                                                     \
})

#define cinq_contains_5(list, value, a_, b_, body) ({                                                   \
    __auto_type cinq__ct_n = (list);                                                                    \
    __typeof__(*cinq__ct_n) cinq__ct_v = (value);                                                       \
    int cinq__ct_found = 0;                                                                             \
    if (cinq__ct_n) {                                                                                   \
        size_t cinq__ct_len = cinq_length(cinq__ct_n);                                                  \
        for (size_t cinq__ct_i = 0; cinq__ct_i < cinq__ct_len; cinq__ct_i++) {                          \
            __auto_type a_ = cinq__ct_n[cinq__ct_i];                                                    \
            __auto_type b_ = cinq__ct_v;                                                                \
            if ((body)) {                                                                               \
                cinq__ct_found = 1;                                                                     \
                break;                                                                                  \
            }                                                                                           \
        }                                                                                               \
    }                                                                                                   \
    cinq__consume(cinq__ct_n);                                                                          \
    cinq__ct_found;                                                                                     \
})

#define cinq_contains(...) cinq__cat(cinq__d_contains_, cinq__n(__VA_ARGS__))(__VA_ARGS__)
#define cinq__d_contains_1(...) cinq__s_contains_1(__VA_ARGS__)
#define cinq__d_contains_2(...) cinq_contains_2(__VA_ARGS__)
#define cinq__d_contains_4(...) cinq__s_contains_4(__VA_ARGS__)
#define cinq__d_contains_5(...) cinq_contains_5(__VA_ARGS__)
#define cinq__x_cinq__s_contains_1(...) cinq_contains_2, ##__VA_ARGS__
#define cinq__x_cinq__s_contains_4(...) cinq_contains_5, ##__VA_ARGS__

/* Distinct */
#define cinq_distinct_1(list) ({                                                                        \
    __auto_type cinq__ds_c = (list);                                                                    \
    __typeof__(cinq__ds_c) cinq__ds_out = NULL;                                                         \
    size_t cinq__ds_len = cinq__safe_len(cinq__ds_c);                                                   \
    for(size_t cinq__ds_i = 0; cinq__ds_i < cinq__ds_len; cinq__ds_i++){                                \
        if(cinq_contains_2(cinq__ds_out, cinq__ds_c[cinq__ds_i])) continue;                             \
        else cinq_append(cinq__ds_out, cinq__ds_c[cinq__ds_i]);                                         \
    }                                                                                                   \
    cinq__consume(cinq__ds_c);                                                                          \
    cinq__ds_out;                                                                                       \
})

#define cinq_distinct_4(list, a_, b_, body) ({                                                          \
    __auto_type cinq__ds_c = (list);                                                                    \
    __typeof__(cinq__ds_c) cinq__ds_out = NULL;                                                         \
    size_t cinq__ds_len = cinq__safe_len(cinq__ds_c);                                                   \
    for(size_t cinq__ds_i = 0; cinq__ds_i < cinq__ds_len; cinq__ds_i++){                                \
        if(cinq_contains_5(cinq__ds_out, cinq__ds_c[cinq__ds_i], a_, b_, body)) continue;               \
        else cinq_append(cinq__ds_out, cinq__ds_c[cinq__ds_i]);                                         \
    }                                                                                                   \
    cinq__consume(cinq__ds_c);                                                                          \
    cinq__ds_out;                                                                                       \
})

#define cinq_distinct(...) cinq__cat(cinq__d_distinct_, cinq__n(__VA_ARGS__))(__VA_ARGS__)
#define cinq__d_distinct_0(...) cinq__s_distinct_0()
#define cinq__d_distinct_1(...) cinq_distinct_1(__VA_ARGS__)
#define cinq__d_distinct_3(...) cinq__s_distinct_3(__VA_ARGS__)
#define cinq__d_distinct_4(...) cinq_distinct_4(__VA_ARGS__)
#define cinq__x_cinq__s_distinct_0(...) cinq_distinct_1, ##__VA_ARGS__
#define cinq__x_cinq__s_distinct_3(...) cinq_distinct_4, ##__VA_ARGS__

/* Any */
#define cinq_any_4(list, item_, index_, body) ({                                                        \
    __auto_type cinq__an_c = (list);                                                                    \
    int cinq__an_out = 0;                                                                               \
    if(cinq__an_c) {                                                                                    \
        for(size_t index_ = 0; index_ < cinq_length(cinq__an_c); index_++){                             \
            __auto_type item_ = cinq__an_c[index_];                                                     \
            if((body)){                                                                                 \
                cinq__an_out = 1;                                                                       \
                break;                                                                                  \
            }                                                                                           \
        }                                                                                               \
    }                                                                                                   \
    cinq__consume(cinq__an_c);                                                                          \
    cinq__an_out;                                                                                       \
})

#define cinq_any_3(list, item_, body) ({                                                                \
    __auto_type cinq__an_c = (list);                                                                    \
    int cinq__an_out = 0;                                                                               \
    if(cinq__an_c) {                                                                                    \
        for(size_t cinq__an_i = 0; cinq__an_i < cinq_length(cinq__an_c); cinq__an_i++){                 \
            __auto_type item_ = cinq__an_c[cinq__an_i];                                                 \
            if((body)){                                                                                 \
                cinq__an_out = 1;                                                                       \
                break;                                                                                  \
            }                                                                                           \
        }                                                                                               \
    }                                                                                                   \
    cinq__consume(cinq__an_c);                                                                          \
    cinq__an_out;                                                                                       \
})

#define cinq_any(...) cinq__cat(cinq__d_any_, cinq__n(__VA_ARGS__))(__VA_ARGS__)
#define cinq__d_any_2(...) cinq__s_any(__VA_ARGS__)
#define cinq__d_any_3(...) cinq_any_3(__VA_ARGS__)
#define cinq__d_any_4(...) cinq_any_4(__VA_ARGS__)
#define cinq__x_cinq__s_any(...) cinq_any_3, ##__VA_ARGS__

/* All */
#define cinq_all_4(list, item_, index_, body) ({                                                        \
    __auto_type cinq__al_c = (list);                                                                    \
    int cinq__al_out = 0;                                                                               \
    if(cinq__al_c) {                                                                                    \
        for(size_t index_ = 0; index_ < cinq_length(cinq__al_c); index_++){                             \
            __auto_type item_ = cinq__al_c[index_];                                                     \
            if((body)){                                                                                 \
                cinq__al_out = 1;                                                                       \
                continue;                                                                               \
            }else{                                                                                      \
                cinq__al_out = 0;                                                                       \
                break;                                                                                  \
            }                                                                                           \
        }                                                                                               \
    }                                                                                                   \
    cinq__consume(cinq__al_c);                                                                          \
    cinq__al_out;                                                                                       \
})

#define cinq_all_3(list, item_, body) ({                                                                \
    __auto_type cinq__al_c = (list);                                                                    \
    int cinq__al_out = 0;                                                                               \
    if(cinq__al_c) {                                                                                    \
        for(size_t cinq__al_i = 0; cinq__al_i < cinq_length(cinq__al_c); cinq__al_i++){                 \
            __auto_type item_ = cinq__al_c[cinq__al_i];                                                 \
            if((body)){                                                                                 \
                cinq__al_out = 1;                                                                       \
                continue;                                                                               \
            }else{                                                                                      \
                cinq__al_out = 0;                                                                       \
                break;                                                                                  \
            }                                                                                           \
        }                                                                                               \
    }                                                                                                   \
    cinq__consume(cinq__al_c);                                                                          \
    cinq__al_out;                                                                                       \
})

#define cinq_all(...) cinq__cat(cinq__d_all_, cinq__n(__VA_ARGS__))(__VA_ARGS__)
#define cinq__d_all_2(...) cinq__s_all(__VA_ARGS__)
#define cinq__d_all_3(...) cinq_all_3(__VA_ARGS__)
#define cinq__d_all_4(...) cinq_all_4(__VA_ARGS__)
#define cinq__x_cinq__s_all(...) cinq_all_3, ##__VA_ARGS__

/* Chain */
#define cinq__chain_2(c, s1)                                                                            \
    cinq__ap(c, s1)
#define cinq__chain_3(c, s1, s2)                                                                        \
    cinq__ap(cinq_tmp(cinq__chain_2(c, s1)), s2)
#define cinq__chain_4(c, s1, s2, s3)                                                                    \
    cinq__ap(cinq_tmp(cinq__chain_3(c, s1, s2)), s3)
#define cinq__chain_5(c, s1, s2, s3, s4)                                                                \
    cinq__ap(cinq_tmp(cinq__chain_4(c, s1, s2, s3)), s4)
#define cinq__chain_6(c, s1, s2, s3, s4, s5)                                                            \
    cinq__ap(cinq_tmp(cinq__chain_5(c, s1, s2, s3, s4)), s5)
#define cinq__chain_7(c, s1, s2, s3, s4, s5, s6)                                                        \
    cinq__ap(cinq_tmp(cinq__chain_6(c, s1, s2, s3, s4, s5)), s6)
#define cinq__chain_8(c, s1, s2, s3, s4, s5, s6, s7)                                                    \
    cinq__ap(cinq_tmp(cinq__chain_7(c, s1, s2, s3, s4, s5, s6)), s7)

#define cinq_chain(...) cinq__cat(cinq__chain_, cinq__n(__VA_ARGS__))(__VA_ARGS__)

/* Except */
/* Map */
/* To Arr */

#endif /* CINQ_H */

#ifdef CINQ_STRIP

#define new(...)       cinq_new(__VA_ARGS__)
#define to_list(...)   cinq_to_list(__VA_ARGS__)
#define append(...)    cinq_append(__VA_ARGS__)
#define list_free(...) cinq_free(__VA_ARGS__)

#define chain(...)     cinq_chain(__VA_ARGS__)

#define select(...)    cinq_select(__VA_ARGS__)
#define each(...)      cinq_each(__VA_ARGS__)
#define take(...)      cinq_take(__VA_ARGS__)
#define where(...)     cinq_where(__VA_ARGS__)
#define slice(...)     cinq_slice(__VA_ARGS__)
#define union(...)     cinq_union(__VA_ARGS__)
#define contains(...)  cinq_contains(__VA_ARGS__)
#define distinct(...)  cinq_distinct(__VA_ARGS__)
#define any(...)       cinq_any(__VA_ARGS__)
#define all(...)       cinq_all(__VA_ARGS__)

#endif