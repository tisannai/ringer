/**
 * @file   ringer.c
 * @author Tero Isannainen <tero.isannainen@gmail.com>
 * @date   Sun Mar 25 14:09:17 2018
 *
 * @brief  Ring buffer (Queue) for pointers.
 *
 */

#include <string.h>
#include "ringer.h"


const char* rg_version = "0.0.1";


/* clang-format off */

/** @cond ringer_none */
#define rg_true  1
#define rg_false 0
#define rg_struct_size(size) ( sizeof(rg_s) + size*sizeof(void*) )
#define rg_unit_size         ( sizeof( void* ) )
#define rg_nth( rg, pos )    rg->data[ ( pos ) ]
/** @endcond ringer_none */

/* clang-format on */


static rg_size_t rg_next_index( rg_size_t size, rg_size_t idx );
static rg_size_t rg_prev_index( rg_size_t size, rg_size_t idx );
static void rg_rotate( rg_t rg, rg_size_t a, rg_size_t m, rg_size_t b );



/* ------------------------------------------------------------
 * Ringer:
 */


rg_t rg_new( rg_size_t size )
{
    rg_t rg;

    rg = (rg_t)rg_malloc( rg_struct_size( size ) );

    rg->ridx = 0;
    rg->widx = 0;
    rg->cnt = 0;
    rg->size = size;

    return rg;
}


void rg_destroy( rg_p rgr )
{
    rg_free( *rgr );
    *rgr = NULL;
}


int rg_put( rg_t rg, void* item )
{
    if ( !rg_is_full( rg ) ) {
        rg_nth( rg, rg->widx ) = item;
        rg->widx = rg_next_index( rg->size, rg->widx );
        rg->cnt++;
        return rg_true;
    } else
        return rg_false;
}


void* rg_get( rg_t rg )
{
    void* item;

    if ( !rg_is_empty( rg ) ) {
        item = rg_nth( rg, rg->ridx );
        rg->ridx = rg_next_index( rg->size, rg->ridx );
        rg->cnt--;
        return item;
    } else
        return NULL;
}


int rg_ram( rg_p rgr, void* item )
{
    int ret;

    if ( rg_is_full( *rgr ) ) {
        rg_resize( rgr, rg_size( *rgr ) * 2 );
        ret = rg_true;
    } else {
        ret = rg_false;
    }

    rg_t rg;
    rg = *rgr;
    rg_nth( rg, rg->widx ) = item;
    rg->widx = rg_next_index( rg->size, rg->widx );
    rg->cnt++;

    return ret;
}


int rg_put_front( rg_t rg, void* item )
{
    if ( !rg_is_full( rg ) ) {
        rg->ridx = rg_prev_index( rg->size, rg->ridx );
        rg_nth( rg, rg->ridx ) = item;
        rg->cnt++;
        return rg_true;
    } else
        return rg_false;
}


void* rg_get_back( rg_t rg )
{
    void* item;

    if ( !rg_is_empty( rg ) ) {
        rg->widx = rg_prev_index( rg->size, rg->widx );
        item = rg_nth( rg, rg->widx );
        rg->cnt--;
        return item;
    } else
        return NULL;
}


void* rg_peek( rg_t rg )
{
    if ( !rg_is_empty( rg ) )
        return rg_nth( rg, rg->ridx );
    else
        return NULL;
}


void* rg_peek_back( rg_t rg )
{
    if ( !rg_is_empty( rg ) ) {
        return rg_nth( rg, rg_prev_index( rg->size, rg->widx ) );
    } else
        return NULL;
}


void* rg_get_nth( rg_t rg, rg_pos_t pos )
{
    void*    item;
    rg_size_t npos;
    rg_size_t idx;

    if ( pos < 0 )
        npos = rg->cnt + pos;
    else
        npos = pos;

    if ( rg_is_empty( rg ) || npos >= rg->cnt )
        return NULL;

    rg->cnt--;

    if ( rg->widx > rg->ridx ) {

        /* ..r-D---w.... */

        idx = rg->ridx + npos;
        item = rg_nth( rg, idx );

        if ( idx != rg->ridx ) {

            memmove( &( rg_nth( rg, idx ) ), &( rg_nth( rg, idx + 1 ) ), ( rg->widx - idx ) * rg_unit_size );
            rg->widx--;

        } else {

            rg->ridx++;

        }

    } else {

        /*
         * ----w...r----
         *   OR
         * --------r----
         *         w
         */

        idx = ( rg->ridx + npos ) % rg->size;
        item = rg_nth( rg, idx );

        if ( idx < rg->widx ) {

            /* -D--w...r---- */

            if ( idx != rg->widx-1 ) {
                memmove( &( rg_nth( rg, idx ) ),
                         &( rg_nth( rg, idx + 1 ) ),
                         ( rg->widx - idx - 1 ) * rg_unit_size );
            }
            rg->widx = rg_prev_index( rg->size, rg->widx );

        } else {

            /* ----w...r-D-- */

            if ( idx != rg->ridx ) {
                memmove( &( rg_nth( rg, rg->ridx + 1 ) ),
                         &( rg_nth( rg, rg->ridx ) ),
                         ( idx - rg->ridx ) * rg_unit_size );
            }
            rg->ridx = rg_next_index( rg->size, rg->ridx );
        }
    }

    return item;
}


rg_size_t rg_count( rg_t rg )
{
    return rg->cnt;
}


int rg_is_empty( rg_t rg )
{
    return rg->cnt == 0;
}


int rg_is_full( rg_t rg )
{
    return ( rg->cnt >= rg->size );
}


rg_size_t rg_size( rg_t rg )
{
    return rg->size;
}


int rg_resize( rg_p rgr, rg_size_t size )
{
    rg_t rg = *rgr;

    if ( size < rg->cnt || size < RG_MIN_SIZE )
        return rg_false;

    if ( rg_is_empty( rg ) ) {

        /* No copying. */
        rg->ridx = 0;
        rg->widx = 0;

    } else {

        /* Pack data to start of storage by rotating. */

        if ( rg->widx <= rg->ridx ) {
            /* ----w...r---- */
            if ( rg->ridx != 0 )
                rg_rotate( rg, 0, rg->ridx, rg->size );
        } else {
            /* ....r---w.... */
            memmove( rg->data, &( rg->data[ rg->ridx ] ), rg->cnt * rg_unit_size );
        }
        rg->ridx = 0;
        rg->widx = rg->cnt % size;
            
        *rgr = (rg_t)rg_realloc( rg, rg_struct_size( size ) );
    }

    (*rgr)->size = size;

    return rg_true;
}




/* ------------------------------------------------------------
 * Internal functions:
 */


static rg_size_t rg_next_index( rg_size_t size, rg_size_t idx )
{
    return ( ( rg_size_t )( idx + 1 ) ) % size;
}


static rg_size_t rg_prev_index( rg_size_t size, rg_size_t idx )
{
    return ( ( rg_size_t )( idx - 1 + size ) ) % size;
}


static void rg_rotate( rg_t rg, rg_size_t a, rg_size_t m, rg_size_t b )
{
    rg_size_t n = m;
    void* swap;

    while ( a != n ) {

        swap = rg_nth( rg, a );
        rg_nth( rg, a ) = rg_nth( rg, n );
        rg_nth( rg, n ) = swap;
        a++;
        n++;

        if ( n == b )
            n = m;
        else if ( a == m )
            m = n;
    }
}



#if 0

#include <stdio.h>

/* Helper function for debugging. */
void rg_show( rg_t rg )
{
    rg_size_t w, r;
    rg_size_t i = 0;
    
    w = rg->widx;
    r = rg->ridx;
    
    printf( "\n\n" );

    if ( rg->cnt == 0 ) {

        while ( i < rg->size ) {
            printf( "%2lu ----", i );
            if ( i == w ) printf( " w" );
            if ( i == r ) printf( " r" );
            printf( "\n" );
            i++;
        }

    } else if ( w <= r ) {

        /* ----w...r---- */

        while ( i < w ) {
            printf( "%2lu %4d\n", i, *( (int*)rg->data[ i ]) );
            i++;
        }

        if ( i == r )
            printf( "%2lu %4d", i, *( (int*)rg->data[ i ]) );
        else
            printf( "%2lu ----", i );
        if ( i == w ) printf( " w" );
        if ( i == r ) printf( " r" );
        printf( "\n" );
        i++;

        while ( i < r ) {
            printf( "%2lu ----\n", i );
            i++;
        }

        printf( "%2lu %4d", i, *( (int*)rg->data[ i ]) );
        if ( i == r ) printf( " r" );
        printf( "\n" );
        i++;

        while ( i < rg->size ) {
            printf( "%2lu %4d\n", i, *( (int*)rg->data[ i ]) );
            i++;
        }
    } else {

        /* ....r---w.... */

        i = 0;
        while ( i < r ) {
            printf( "%2lu ----\n", i );
            i++;
        }

        printf( "%2lu %4d", i, *( (int*)rg->data[ i ]) );
        if ( i == w ) printf( " w" );
        if ( i == r ) printf( " r" );
        printf( "\n" );
        i++;

        while ( i < w ) {
            printf( "%2lu %4d\n", i, *( (int*)rg->data[ i ]) );
            i++;
        }

        printf( "%2lu ----", i );
        if ( i == w ) printf( " w" );
        printf( "\n" );
        i++;

        while ( i < rg->size ) {
            printf( "%2lu ----\n", i );
            i++;
        }
    }
}

#endif
