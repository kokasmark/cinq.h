/*

  oooooooo8 ooooo oooo   oooo  ooooooo
o888     88  888   8888o  88 o888   888o
888          888   88 888o88 888     888
888o     oo  888   88   8888 888o  8o888
 888oooo88  o888o o88o    88   88ooo88
                                    88o8
           C Integrated Query         888


Single header library to perform Linq (Language-Integrated Query) operations on Dynamic Arrays.

(c) Kokas Mark

*/

#include <stddef.h>


/* Dynamic Array */

#define cinq__meta(ptr)        ((size_t *)(ptr) - 2)
#define cinq_capacity(ptr)     (cinq__meta(ptr)[0])
#define cinq_length(ptr)       (cinq__meta(ptr)[1])

static inline void *cinq_new(size_t elem_size) {
    size_t initial_cap = 4;
    size_t *meta = malloc(2 * sizeof(size_t) + elem_size * initial_cap);
    meta[0] = initial_cap; // capacity
    meta[1] = 0;           // length
    return meta + 2;
}

static inline void *cinq_reserve(void *list, size_t elem_size, size_t n) {
	unsigned int c = n;
	c--;
	c |= c >> 1;
	c |= c >> 2;
	c |= c >> 4;
	c |= c >> 8;
	c |= c >> 16;
	c++;
	size_t new_cap = c;

    size_t *old_meta = cinq__meta(list);
    size_t *new_meta = realloc(
        old_meta,
        2 * sizeof(size_t) + new_cap * elem_size
    );

    new_meta[0] = new_cap;
	new_meta[1] = old_meta[1];
    return new_meta + 2;
}

static inline void *cinq_grow(void *list, size_t elem_size) {
    size_t oldc_ap = cinq_capacity(list);
    size_t new_cap = oldc_ap * 2;

    size_t *old_meta = cinq__meta(list);
    size_t *new_meta = realloc(
        old_meta,
        2 * sizeof(size_t) + new_cap * elem_size
    );

    new_meta[0] = new_cap;
    return new_meta + 2;
}

#define cinqc_onvert(array, length)									  \
	if((array) && (length) > 0){									  \
		(list) = cinq_new(sizeof(*(array)));						  \
		cinq__meta(list)[1] = length;					  			  \
	}

#define cinq_append(list, item) do {                                  \
    if ((list) == NULL) {                                             \
        (list) = cinq_new(sizeof(*(list)));               			  \
    }                                                                 \
    size_t cap = cinq_capacity(list);                      			  \
    size_t len = cinq_length(list);                        			  \
    if (len == cap) {                                                 \
        (list) = cinq_grow(list, sizeof(*(list)));       			  \
    }                                                                 \
    (list)[ cinq_length(list)++ ] = (item);                			  \
} while (0)

/* ----------------------------------------------------------------- */
/* Operations */

#define cinq_get_macro(_1,_2,_3,_4,n_ame,...) n_ame

/* Select */

#define cinq_select_4(list, item_, index_, body) ({                 \
	__auto_type c_ = (list);									   \
	__typeof__(list) mapped__ = NULL;							   \
	if(c_) {														   \
		for(size_t index_ = 0; index_ < cinq_length(c_); index_++){	   \
			__auto_type item_ = c_[index_];							   \
			cinq_append(mapped__, (body));							   \
		}															   \
	}																   \
	mapped__;														   \
})

#define cinq_select_3(list, item_, body) ({                         \
    __auto_type c_ = (list);                                     \
    __typeof__(list) mapped__ = NULL;                                  \
    if(c_) {                                                           \
        for(size_t i_ = 0; i_ < cinq_length(c_); i_++){                \
            __auto_type item_ = c_[i_];                                \
            cinq_append(mapped__, (body));                             \
        }                                                              \
    }                                                                  \
    mapped__;                                                          \
})

#define cinq_select(...)                                                  \
    cinq_get_macro(__VA_ARGS__, cinq_select_4, cinq_select_3)(__VA_ARGS__)

/* Each */

#define cinq_each_4(list, item_, index_, body) ({                     \
    __auto_type c_ = (list);                                          \
    if(c_) {                                                          \
        for(size_t index_ = 0; index_ < cinq_length(c_); index_++){   \
            __auto_type item_ = c_[index_];                           \
            body;                                                     \
        }                                                             \
    }                                                                 \
})

#define cinq_each_3(list, item_, body) ({                             \
    __auto_type c_ = (list);                                          \
    if(c_) {                                                          \
        for(size_t i_ = 0; i_ < cinq_length(c_); i_++){               \
            __auto_type item_ = c_[i_];                               \
            body;                                                     \
        }                                                             \
    }                                                                 \
})

#define cinq_each(...) \
    cinq_get_macro(__VA_ARGS__, cinq_each_4, cinq_each_3)(__VA_ARGS__)

/* Where */

#define cinq_where_4(list, item_, index_, body) ({					   \
	__auto_type c_ = (list);                                     	   \
    __typeof__(list) filtered__ = NULL;								   \
	if(c_) {														   \
		for(size_t index_ = 0; index_ < cinq_length(c_); index_++){    \
            __auto_type item_ = c_[index_];                            \
			if((body)){												   \
            	cinq_append(filtered__, item_);                        \
			}														   \
        }															   \
	}																   \
	filtered__;														   \
})

#define cinq_where_3(list, item_, body) ({		                       \
    __auto_type c_ = (list);                                           \
    __typeof__(list) filtered__ = NULL;                                \
    if(c_) {                                                           \
        for(size_t i_ = 0; i_ < cinq_length(c_); i_++){    			   \
            __auto_type item_ = c_[i_];	                               \
            if((body)){                                                \
                cinq_append(filtered__, item_);                        \
            }                                                          \
        }                                                              \
    }                                                                  \
    filtered__;                                                        \
})

#define cinq_where(...)                                                \
    cinq_get_macro(__VA_ARGS__, cinq_where_4, cinq_where_3)(__VA_ARGS__)

/* Take */
#define cinq_take(list, n) ({                                          \
    __auto_type c_ = (list);                                           \
    size_t len_ = cinq_length(c_);                                     \
    size_t n_ = (n) > len_ ? len_ : (n);                               \
                                                                       \
    __typeof__(list) out_ = NULL;                                      \
    if(n_ > 0) {                                                       \
		out_ = cinq_new(sizeof(*c_));                                  \
        out_ = cinq_reserve(out_, sizeof(*c_), n_);                    \
        memcpy(out_, c_, n_ * sizeof(*c_));                            \
		cinq__meta(out_)[1] = n_;									   \
    }                                                                  \
    out_;                                                              \
})

/* Slice */
#define cinq_slice(list, start, end) ({                                \
    __auto_type c_ = (list);                                           \
    __typeof__(list) out_ = NULL;                                      \
                                                                       \
    size_t len_ = cinq_length(c_);                                     \
    size_t s_ = (start);                                               \
    size_t e_ = (end);                                                 \
                                                                       \
    if (s_ <= e_ && e_ <= len_) {                                      \
        size_t n_ = e_ - s_;                                           \
        out_ = cinq_new(sizeof(*c_));                                  \
        out_ = cinq_reserve(out_, sizeof(*c_), n_);                    \
        memcpy(out_,                                                   \
               (char*)c_ + s_ * sizeof(*c_),                           \
               n_ * sizeof(*c_));                                      \
		cinq__meta(out_)[1] = n_;									   \
    }                                                                  \
                                                                       \
    out_;                                                              \
})

/* Union */
#define cinq_union(listA, listB) ({                                      \
    __auto_type a_ = (listA);                                            \
    __auto_type b_ = (listB);                                            \
    __typeof__(listA) out_ = NULL;                                       \
                                                                         \
    if (sizeof(*a_) == sizeof(*b_)) {                                    \
        size_t lenA_ = cinq_length(a_);                                  \
        size_t lenB_ = cinq_length(b_);                                  \
        size_t total_ = lenA_ + lenB_;                                   \
                                                                         \
		out_ = cinq_new(sizeof(*a_));                                    \
        out_ = cinq_reserve(out_, sizeof(*a_), total_);                  \
                                                                         \
        memcpy(out_, a_, lenA_ * sizeof(*a_));                           \
        memcpy((char*)out_ + lenA_ * sizeof(*a_),                        \
               b_, lenB_ * sizeof(*b_));                                 \
		cinq__meta(out_)[1] = total_;	    						     \
    }                                                                    \
                                                                         \
    out_;                                                                \
})

/* Contains */
/* TODO: Fix bug where both list[i] and value doesnt change and always a = a*/
#define cinq_contains(list, value) ({                     \
    __auto_type c_ = (list);                              \
    int found_ = 0;                                       \
    if (c_) {                                             \
        size_t len_ = cinq_length(c_);                    \
        for (size_t i_ = 0; i_ < len_; i_++) {            \
            if (c_[i_] == value && !memcmp(&c_[i_], &(value), sizeof(value))) {\
                found_ = 1;                               \
                break;                                    \
            }                                             \
        }                                                 \
    }                                                     \
    found_;                                               \
})

/* Distinct */
#define cinq_distinct(list) ({                                        \
	__auto_type c_ = (list);										  \
	__typeof__(list) out_ = NULL;									  \
	for(int i_ = 0; i_ < cinq_length(c_); i_++){		    		  \
		if(cinq_contains(out_, c_[i_])) continue;         			  \
		else cinq_append(out_, c_[i_]);                               \
	}																  \
	out_;														      \
})

/* Except */
/* Reduce */
/* Any */
#define cinq_any_4(list, item_, index_, body) ({                      \
    __auto_type c_ = (list);                                          \
	int out_ = 0;                                                     \
    if(c_) {                                                          \
        for(size_t index_ = 0; index_ < cinq_length(c_); index_++){   \
            __auto_type item_ = c_[index_];                           \
            if((body)){                                               \
				out_ = 1;								              \
				break;												  \
			}														  \
        }                                                             \
    }                                                                 \
	out_;															  \
})

#define cinq_any_3(list, item_, body) ({                              \
    __auto_type c_ = (list);                                          \
    int out_ = 0;                                                     \
    if(c_) {                                                          \
        for(size_t i_ = 0; i_ < cinq_length(c_); i_++){               \
            __auto_type item_ = c_[i_];                               \
            if((body)){                                               \
                out_ = 1;                                             \
                break;                                                \
            }                                                         \
        }                                                             \
    }                                                                 \
    out_;															  \
})

#define cinq_any(...) \
    cinq_get_macro(__VA_ARGS__, cinq_any_4, cinq_any_3)(__VA_ARGS__)

/* All */
#define cinq_all_4(list, item_, index_, body) ({                      \
    __auto_type c_ = (list);                                          \
    int out_ = 0;                                                     \
    if(c_) {                                                          \
        for(size_t index_ = 0; index_ < cinq_length(c_); index_++){   \
            __auto_type item_ = c_[index_];                           \
            if((body)){                                               \
                out_ = 1;                                             \
                continue;                                             \
            }else{                                                    \
				out_ = 0;								  			  \
				break;												  \
			}														  \
        }                                                             \
    }                                                                 \
    out_;                                                             \
})

#define cinq_all_3(list, item_, body) ({                              \
    __auto_type c_ = (list);                                          \
    int out_ = 0;                                                     \
    if(c_) {                                                          \
        for(size_t i_ = 0; i_ < cinq_length(c_); i_++){               \
            __auto_type item_ = c_[i_];                               \
            if((body)){                                               \
                out_ = 1;                                             \
                continue;                                             \
            }else{                                                    \
                out_ = 0;                                             \
                break;                                                \
            }                                                         \
        }                                                             \
    }                                                                 \
    out_;															  \
})

#define cinq_all(...) \
    cinq_get_macro(__VA_ARGS__, cinq_all_4, cinq_all_3)(__VA_ARGS__)
