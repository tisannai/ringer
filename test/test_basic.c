#include "unity.h"
#include "ringer.h"


void gdb_breakpoint( void )
{
}


void test_basics( void )
{
    rg_t rg;
    int limit;

    limit = 10;

    rg = rg_new( limit );
    TEST_ASSERT_EQUAL( limit, rg_size( rg ) );
    TEST_ASSERT_EQUAL( 1, rg_is_empty( rg ) );
    TEST_ASSERT_EQUAL( 0, rg_is_full( rg ) );
    

    int items[ limit + 1 ];
    int* item;

    for ( int i = 0; i < limit-1; i++ ) {
        items[ i ] = i;
        rg_put( rg, &( items[ i ] ) );
    }
    items[ limit-1 ] = limit-1;

    TEST_ASSERT_EQUAL( 0, rg_is_empty( rg ) );
    TEST_ASSERT_EQUAL( 0, rg_is_full( rg ) );

    rg_put( rg, &( items[ limit-1 ] ) );
    
    TEST_ASSERT_EQUAL( 0, rg_is_empty( rg ) );
    TEST_ASSERT_EQUAL( 1, rg_is_full( rg ) );
    TEST_ASSERT_EQUAL( limit, rg_count( rg ) );

    for ( int i = 0; i < limit; i++ ) {
        item = rg_get( rg );
        TEST_ASSERT_EQUAL( i, *item );
    }

    rg_destroy( &rg );
}


void test_resize( void )
{
    rg_t rg;
    int limit;

    limit = 4;

    rg = rg_new( limit );
    TEST_ASSERT_EQUAL( limit, rg_size( rg ) );
    TEST_ASSERT_EQUAL( 1, rg_is_empty( rg ) );
    TEST_ASSERT_EQUAL( 0, rg_is_full( rg ) );
    
    int items[ 16*limit ];
    int* item;
    int w, r;

    w = 0;
    r = 0;

    for ( int i = 0; i < 16*limit; i++ ) {
        items[ i ] = i;
    }

    for ( int i = 0; i < limit; i++ ) {
        rg_put( rg, &( items[ w++ ] ) );
    }

    TEST_ASSERT_EQUAL( limit, rg_size( rg ) );
    TEST_ASSERT_EQUAL( 1, rg_is_full( rg ) );

    for ( int i = 0; i < limit/2; i++ ) {
        item = rg_get( rg );
        TEST_ASSERT_EQUAL( items[ r++ ], *item );
    }

    for ( int i = 0; i < limit; i++ ) {
        rg_ram( &rg, &( items[ w++ ] ) );
    }

    TEST_ASSERT_EQUAL( 2*limit, rg_size( rg ) );

    for ( int i = 0; i < limit/2+limit; i++ ) {
        item = rg_get( rg );
        TEST_ASSERT_EQUAL( items[ r++ ], *item );
    }
    TEST_ASSERT_EQUAL( 1, rg_is_empty( rg ) );

    rg_resize( &rg, rg_size( rg ) / 4 );

    for ( int i = 0; i < limit/2; i++ ) {
        rg_put( rg, &( items[ w++ ] ) );
    }

    TEST_ASSERT_EQUAL( limit/2, rg_size( rg ) );
    TEST_ASSERT_EQUAL( 1, rg_is_full( rg ) );

    for ( int i = 0; i < limit/2; i++ ) {
        item = rg_get( rg );
        TEST_ASSERT_EQUAL( items[ r++ ], *item );
    }

    rg_destroy( &rg );
}


void test_abnormal( void )
{
    rg_t rg;
    int limit;

    limit = 4;
    rg = rg_new( limit );

    int items[ limit + 1 ];
    int* item;

    for ( int i = 0; i < limit; i++ ) {
        items[ i ] = i;
    }

    /* 1...
     * r
     *  w
     */
    rg_put( rg, &( items[1] ) );
    item = rg_peek( rg );
    TEST_ASSERT_EQUAL( items[1], *item );
    TEST_ASSERT_EQUAL( 0, rg->ridx );
    TEST_ASSERT_EQUAL( 1, rg->widx );

    /* 1..2
     *    r
     *  w
     */
    rg_put_front( rg, &( items[2] ) );
    item = rg_peek( rg );
    TEST_ASSERT_EQUAL( items[2], *item );
    TEST_ASSERT_EQUAL( limit-1, rg->ridx );
    TEST_ASSERT_EQUAL( 1, rg->widx );

    item = rg_peek_back( rg );
    TEST_ASSERT_EQUAL( items[1], *item );

    item = rg_get_back( rg );
    TEST_ASSERT_EQUAL( items[1], *item );
    TEST_ASSERT_EQUAL( limit-1, rg->ridx );
    TEST_ASSERT_EQUAL( 0, rg->widx );

    /* ...2
     *    r
     * w
     */

    rg_put( rg, &( items[0] ) );

    /* 0..2
     *    r
     *  w
     */

    item = rg_get_nth( rg, 1 );
    TEST_ASSERT_EQUAL( items[0], *item );

    /* ...2
     *    r
     * w
     */

    rg_put( rg, &( items[0] ) );
    rg_get( rg );

    rg_put( rg, &( items[1] ) );
    rg_get( rg );
    rg_put( rg, &( items[2] ) );


    item = rg_get_nth( rg, -1 );
    TEST_ASSERT_EQUAL( items[2], *item );

    item = rg_get_nth( rg, 0 );
    TEST_ASSERT_EQUAL( items[1], *item );

    rg_put( rg, &( items[1] ) );
    rg_put( rg, &( items[2] ) );
    rg_put( rg, &( items[3] ) );
    rg_put( rg, &( items[1] ) );

    item = rg_get_nth( rg, 0 );
    item = rg_get_nth( rg, 1 );
    TEST_ASSERT_EQUAL( items[3], *item );

    item = rg_get_nth( rg, 0 );
    item = rg_get_nth( rg, 0 );

    rg_put( rg, &( items[2] ) );
    rg_put( rg, &( items[3] ) );
    rg_put( rg, &( items[2] ) );

    int size;
    size = rg_size( rg );
    rg_resize( &rg, size/2 );
    TEST_ASSERT_EQUAL( size, rg_size( rg ) );
    
    item = rg_get_nth( rg, 1 );
    TEST_ASSERT_EQUAL( items[3], *item );

    item = rg_get_nth( rg, 0 );
    item = rg_get_nth( rg, 0 );

    rg_put( rg, &( items[2] ) );
    rg_put( rg, &( items[3] ) );
    rg_put( rg, &( items[2] ) );
    item = rg_get_nth( rg, 0 );
    
    rg_resize( &rg, size/2 );

    item = rg_get_nth( rg, 1 );
    TEST_ASSERT_EQUAL( items[2], *item );
    item = rg_get_nth( rg, 0 );
    TEST_ASSERT_EQUAL( items[3], *item );

    item = rg_get( rg );
    TEST_ASSERT_EQUAL( NULL, item );

    item = rg_peek( rg );
    TEST_ASSERT_EQUAL( NULL, item );
    
    item = rg_get_back( rg );
    TEST_ASSERT_EQUAL( NULL, item );

    item = rg_peek_back( rg );
    TEST_ASSERT_EQUAL( NULL, item );
    
    item = rg_get_nth( rg, 0 );
    TEST_ASSERT_EQUAL( NULL, item );
    

    rg_put( rg, &( items[1] ) );
    rg_put( rg, &( items[0] ) );
    rg_put( rg, &( items[2] ) );
    rg_put( rg, &( items[3] ) );
    
    TEST_ASSERT_EQUAL( 0, rg_put( rg, &( items[3] ) ) );
    TEST_ASSERT_EQUAL( 0, rg_put_front( rg, &( items[3] ) ) );


//    rg_show( rg );

    rg_destroy( &rg );
}


int rand_within( int limit )
{
    if ( limit > 0 )
        return ( rand() % limit );
    else
        return 0;
}


void check_value( void* item )
{
    if ( item == NULL || *((int*)item) == 0 || *((int*)item) == 1 ) {
        TEST_ASSERT( 1 );
    } else {
        TEST_ASSERT( 0 );
    }
}



void test_random( void )
{
    rg_t rg;
    int r1, r2;
    int items[ 2 ];
    int* item;
    int range = 7;
    int sizes = range - 1;
    int size_hit[ sizes ];
    int rnd;
    int success;


    srand( 1234 );
    
    items[ 0 ] = 0;
    items[ 1 ] = 1;

    for ( int i = 0; i < sizes; i++ ) {
        size_hit[ i ] = 0;
    }

    rnd = 0;

    for (;;) {
        
        rnd++;

        r1 = rand_within( range ) + RG_MIN_SIZE;
        rg = rg_new( r1 );

//        printf( "%4d %4d\n", rnd, r1 );
        
        size_hit[ r1-RG_MIN_SIZE ]++;

        r2 = rand_within( range ) + r1;
        for ( int j = 0; j < r2; j++ ) {
            rg_put( rg, &items[ rand_within( 2 ) ] );
        }

        r2 = rand_within( range ) + r1;
        for ( int j = 0; j < r2; j++ ) {
            item = rg_get( rg);
            check_value( item );
        }

        success = 1;
        for ( int i = 0; i < sizes; i++ ) {
            if ( size_hit[ i ] < range*2 ) {
                success = 0;
                break;
            }
        }

        rg_destroy( &rg );

        if ( success )
            break;
    }


    rnd = 0;

    for ( int i = 0; i < sizes; i++ ) {
        size_hit[ i ] = 0;
    }

    for (;;) {
        
        rnd++;

        r1 = rand_within( range ) + RG_MIN_SIZE;
        rg = rg_new( r1 );

//        printf( "%4d %4d\n", rnd, r1 );
        
        size_hit[ r1-RG_MIN_SIZE ]++;

        r2 = rand_within( range ) + r1;
        for ( int j = 0; j < r2*4; j++ ) {
            rg_ram( &rg, &items[ rand_within( 2 ) ] );
        }

        r2 = rand_within( range ) + r1;
        for ( int j = 0; j < r2; j++ ) {
            item = rg_get( rg);
            check_value( item );
        }

        r2 = rand_within( range ) + r1;
        for ( int j = 0; j < r2*4; j++ ) {
            rg_ram( &rg, &items[ rand_within( 2 ) ] );
        }

        r2 = rand_within( range ) + r1;
        for ( int j = 0; j < r2; j++ ) {
            item = rg_get( rg);
            check_value( item );
        }

        success = 1;
        for ( int i = 0; i < sizes; i++ ) {
            if ( size_hit[ i ] < range*2 ) {
                success = 0;
                break;
            }
        }

        rg_destroy( &rg );

        if ( success )
            break;
    }
}
