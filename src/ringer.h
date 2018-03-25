#ifndef RINGER_H
#define RINGER_H

/**
 * @file   ringer.h
 * @author Tero Isannainen <tero.isannainen@gmail.com>
 * @date   Sun Mar 25 14:09:17 2018
 *
 * @brief  Ring buffer (Queue) for pointers.
 *
 * Ringer supports static and dynamic sized buffers with automatic
 * memory management.
 *
 */

#include <stdlib.h>
#include <stdint.h>


/** Ringer version. */
extern const char* rg_version;


/** Ringer minimum size. */
#define RG_MIN_SIZE 2


/** Size type. */
typedef uint64_t rg_size_t;

/** Position type. */
typedef int64_t rg_pos_t;


/**
 * Ringer struct.
 */
struct rg_struct_s
{
    rg_size_t ridx;      /**< Read index. */
    rg_size_t widx;      /**< Write index. */
    rg_size_t cnt;       /**< Item count. */
    rg_size_t size;      /**< Reservation size for data. */
    void*     data[ 0 ]; /**< Pointer array. */
};
typedef struct rg_struct_s rg_s; /**< Ringer struct. */
typedef rg_s*              rg_t; /**< Ringer pointer. */
typedef rg_t*              rg_p; /**< Ringer pointer reference. */


#ifdef RINGER_MEM_API

/*
 * RINGER_MEM_API allows to use custom memory allocation functions,
 * instead of the default: rg_malloc, rg_free, rg_realloc.
 *
 * If RINGER_MEM_API is used, the user must provide implementation for the
 * below functions and they must be compatible with malloc etc.
 */

extern void* rg_malloc( size_t size );
extern void rg_free( void* ptr );
extern void* rg_realloc( void* ptr, size_t size );

#else

/* Default to common memory management functions. */

/** Reserve memory. */
#define rg_malloc malloc

/** Release memory. */
#define rg_free free

/** Re-reserve memory. */
#define rg_realloc realloc

#endif


/* ------------------------------------------------------------
 * Ringer:
 */


/**
 * Create Ringer with size.
 *
 * @param size Initial size;
 *
 * @return Ringer.
 */
rg_t rg_new( rg_size_t size );


/**
 * Destroy Ringer.
 *
 * @param rgr Ringer reference.
 */
void rg_destroy( rg_p rgr );


/**
 * Put item to Ringer.
 *
 * @param rg   Ringer.
 * @param item Item.
 *
 * @return 1 on success (0 if full).
 */
int rg_put( rg_t rg, void* item );


/**
 * Get item from Ringer.
 *
 * @param rg Ringer.
 *
 * @return Item (or NULL if empty).
 */
void* rg_get( rg_t rg );


/**
 * Forcefully put item to Ringer.
 *
 * If Ringer is full, it will be resized to double. Since Ringer is
 * potentially reallocated, Ringer reference is provided to function.
 *
 * @param rgr  Ringer reference.
 * @param item Item.
 *
 * @return 1 if resized (else 0).
 */
int rg_ram( rg_p rgr, void* item );


/**
 * Put item to front of Ringer.
 *
 * This operation deviates from normal queueing.
 *
 * @param rg   Ringer.
 * @param item Item.
 *
 * @return 1 on success (0 if full).
 */
int rg_put_front( rg_t rg, void* item );


/**
 * Get item from back of Ringer.
 *
 * This operation deviates from normal queueing.
 *
 * @param rg Ringer.
 *
 * @return Item (or NULL if emtpy).
 */
void* rg_get_back( rg_t rg );


/**
 * Peek item from Ringer.
 *
 * No changes to Ringer state.
 *
 * @param rg Ringer.
 *
 * @return Item (or NULL if empty).
 */
void* rg_peek( rg_t rg );


/**
 * Peek item from back of Ringer.
 *
 * No changes to Ringer state.
 *
 * @param rg Ringer.
 *
 * @return Item (or NULL if empty).
 */
void* rg_peek_back( rg_t rg );


/**
 * Get nth item from Ringer.
 *
 * Offset by pos from Read Index. 0 means normal get, and positive
 * offsets are away from read index. Negative indeces refer to back of
 * Ringer.
 *
 * @param rg  Ringer.
 * @param pos Offset from Read Index.
 *
 * @return Item (or NULL).
 */
void* rg_get_nth( rg_t rg, rg_pos_t pos );


/**
 * Return item count of Ringer.
 *
 * @param rg Ringer.
 *
 * @return Count.
 */
rg_size_t rg_count( rg_t rg );


/**
 * Is Ringer empty?
 *
 * @param rg Ringer.
 *
 * @return 1 if empty.
 */
int rg_is_empty( rg_t rg );


/**
 * Is Ringer full?
 *
 * @param rg Ringer.
 *
 * @return 1 if full.
 */
int rg_is_full( rg_t rg );


/**
 * Return Ringer storage size.
 *
 * @param rg Ringer.
 *
 * @return Size.
 */
rg_size_t rg_size( rg_t rg );


/**
 * Resize Ringer to size.
 *
 * Resizing is not done if size is too small. It can be below minimum
 * size for Ringer or current item count is not satisfied.
 *
 * @param rgr  Ringer reference.
 * @param size New size.
 *
 * @return 1 on success (else 0).
 */
int rg_resize( rg_p rgr, rg_size_t size );


#endif
