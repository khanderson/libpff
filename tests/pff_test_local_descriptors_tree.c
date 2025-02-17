/*
 * Library local_descriptors_tree type test program
 *
 * Copyright (C) 2008-2021, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <file_stream.h>
#include <types.h>

#if defined( HAVE_STDLIB_H ) || defined( WINAPI )
#include <stdlib.h>
#endif

#include "pff_test_functions.h"
#include "pff_test_libbfio.h"
#include "pff_test_libcerror.h"
#include "pff_test_libfdata.h"
#include "pff_test_libpff.h"
#include "pff_test_macros.h"
#include "pff_test_memory.h"
#include "pff_test_unused.h"

#include "../libpff/libpff_io_handle.h"
#include "../libpff/libpff_local_descriptors_tree.h"
#include "../libpff/libpff_offsets_index.h"

#if defined( __GNUC__ ) && !defined( LIBPFF_DLL_IMPORT )

/* Tests the libpff_local_descriptors_tree_initialize function
 * Returns 1 if successful or 0 if not
 */
int pff_test_local_descriptors_tree_initialize(
     void )
{
	libcerror_error_t *error                                = NULL;
	libpff_io_handle_t *io_handle                           = NULL;
	libpff_local_descriptors_tree_t *local_descriptors_tree = NULL;
	libpff_offsets_index_t *offsets_index                   = NULL;
	int result                                              = 0;

#if defined( HAVE_PFF_TEST_MEMORY )
	int number_of_malloc_fail_tests                         = 1;
	int number_of_memset_fail_tests                         = 1;
	int test_number                                         = 0;
#endif

	/* Initialize test
	 */
	result = libpff_io_handle_initialize(
	          &io_handle,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	PFF_TEST_ASSERT_IS_NOT_NULL(
	 "io_handle",
	 io_handle );

	PFF_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	result = libpff_offsets_index_initialize(
	          &offsets_index,
	          io_handle,
	          NULL,
	          NULL,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	PFF_TEST_ASSERT_IS_NOT_NULL(
	 "offsets_index",
	 offsets_index );

	PFF_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	/* Test regular cases
	 */
	result = libpff_local_descriptors_tree_initialize(
	          &local_descriptors_tree,
	          io_handle,
	          offsets_index,
	          0,
	          0,
	          0,
	          0,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	PFF_TEST_ASSERT_IS_NOT_NULL(
	 "local_descriptors_tree",
	 local_descriptors_tree );

	PFF_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	result = libpff_local_descriptors_tree_free(
	          &local_descriptors_tree,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	PFF_TEST_ASSERT_IS_NULL(
	 "local_descriptors_tree",
	 local_descriptors_tree );

	PFF_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	/* Test error cases
	 */
	result = libpff_local_descriptors_tree_initialize(
	          NULL,
	          io_handle,
	          offsets_index,
	          0,
	          0,
	          0,
	          0,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 -1 );

	PFF_TEST_ASSERT_IS_NOT_NULL(
	 "error",
	 error );

	libcerror_error_free(
	 &error );

	local_descriptors_tree = (libpff_local_descriptors_tree_t *) 0x12345678UL;

	result = libpff_local_descriptors_tree_initialize(
	          &local_descriptors_tree,
	          io_handle,
	          offsets_index,
	          0,
	          0,
	          0,
	          0,
	          &error );

	local_descriptors_tree = NULL;

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 -1 );

	PFF_TEST_ASSERT_IS_NOT_NULL(
	 "error",
	 error );

	libcerror_error_free(
	 &error );

#if defined( HAVE_PFF_TEST_MEMORY )

	for( test_number = 0;
	     test_number < number_of_malloc_fail_tests;
	     test_number++ )
	{
		/* Test libpff_local_descriptors_tree_initialize with malloc failing
		 */
		pff_test_malloc_attempts_before_fail = test_number;

		result = libpff_local_descriptors_tree_initialize(
		          &local_descriptors_tree,
		          io_handle,
		          offsets_index,
		          0,
		          0,
		          0,
		          0,
		          &error );

		if( pff_test_malloc_attempts_before_fail != -1 )
		{
			pff_test_malloc_attempts_before_fail = -1;

			if( local_descriptors_tree != NULL )
			{
				libpff_local_descriptors_tree_free(
				 &local_descriptors_tree,
				 NULL );
			}
		}
		else
		{
			PFF_TEST_ASSERT_EQUAL_INT(
			 "result",
			 result,
			 -1 );

			PFF_TEST_ASSERT_IS_NULL(
			 "local_descriptors_tree",
			 local_descriptors_tree );

			PFF_TEST_ASSERT_IS_NOT_NULL(
			 "error",
			 error );

			libcerror_error_free(
			 &error );
		}
	}
	for( test_number = 0;
	     test_number < number_of_memset_fail_tests;
	     test_number++ )
	{
		/* Test libpff_local_descriptors_tree_initialize with memset failing
		 */
		pff_test_memset_attempts_before_fail = test_number;

		result = libpff_local_descriptors_tree_initialize(
		          &local_descriptors_tree,
		          io_handle,
		          offsets_index,
		          0,
		          0,
		          0,
		          0,
		          &error );

		if( pff_test_memset_attempts_before_fail != -1 )
		{
			pff_test_memset_attempts_before_fail = -1;

			if( local_descriptors_tree != NULL )
			{
				libpff_local_descriptors_tree_free(
				 &local_descriptors_tree,
				 NULL );
			}
		}
		else
		{
			PFF_TEST_ASSERT_EQUAL_INT(
			 "result",
			 result,
			 -1 );

			PFF_TEST_ASSERT_IS_NULL(
			 "local_descriptors_tree",
			 local_descriptors_tree );

			PFF_TEST_ASSERT_IS_NOT_NULL(
			 "error",
			 error );

			libcerror_error_free(
			 &error );
		}
	}
#endif /* defined( HAVE_PFF_TEST_MEMORY ) */

	/* Clean up
	 */
	result = libpff_offsets_index_free(
	          &offsets_index,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	PFF_TEST_ASSERT_IS_NULL(
	 "offsets_index",
	 offsets_index );

	PFF_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	result = libpff_io_handle_free(
	          &io_handle,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	PFF_TEST_ASSERT_IS_NULL(
	 "io_handle",
	 io_handle );

	PFF_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	return( 1 );

on_error:
	if( error != NULL )
	{
		libcerror_error_free(
		 &error );
	}
	if( local_descriptors_tree != NULL )
	{
		libpff_local_descriptors_tree_free(
		 &local_descriptors_tree,
		 NULL );
	}
	if( offsets_index != NULL )
	{
		libpff_offsets_index_free(
		 &offsets_index,
		 NULL );
	}
	if( io_handle != NULL )
	{
		libpff_io_handle_free(
		 &io_handle,
		 NULL );
	}
	return( 0 );
}

/* Tests the libpff_local_descriptors_tree_free function
 * Returns 1 if successful or 0 if not
 */
int pff_test_local_descriptors_tree_free(
     void )
{
	libcerror_error_t *error = NULL;
	int result               = 0;

	/* Test error cases
	 */
	result = libpff_local_descriptors_tree_free(
	          NULL,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 -1 );

	PFF_TEST_ASSERT_IS_NOT_NULL(
	 "error",
	 error );

	libcerror_error_free(
	 &error );

	return( 1 );

on_error:
	if( error != NULL )
	{
		libcerror_error_free(
		 &error );
	}
	return( 0 );
}

/* Tests the libpff_local_descriptors_tree_get_leaf_node_by_identifier function
 * Returns 1 if successful or 0 if not
 */
int pff_test_local_descriptors_tree_get_leaf_node_by_identifier(
     void )
{
	uint8_t data[ 128 ];

	libbfio_handle_t *file_io_handle                        = NULL;
	libcerror_error_t *error                                = NULL;
	libpff_io_handle_t *io_handle                           = NULL;
	libpff_local_descriptors_tree_t *local_descriptors_tree = NULL;
	libpff_offsets_index_t *offsets_index                   = NULL;
	int result                                              = 0;

	/* Initialize test
	 */
	result = libpff_io_handle_initialize(
	          &io_handle,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	PFF_TEST_ASSERT_IS_NOT_NULL(
	 "io_handle",
	 io_handle );

	PFF_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	result = libpff_offsets_index_initialize(
	          &offsets_index,
	          io_handle,
	          NULL,
	          NULL,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	PFF_TEST_ASSERT_IS_NOT_NULL(
	 "offsets_index",
	 offsets_index );

	PFF_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	result = libpff_local_descriptors_tree_initialize(
	          &local_descriptors_tree,
	          io_handle,
	          offsets_index,
	          0,
	          0,
	          0,
	          0,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	PFF_TEST_ASSERT_IS_NOT_NULL(
	 "local_descriptors_tree",
	 local_descriptors_tree );

	PFF_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	/* Initialize file IO handle
	 */
/* TODO point to test data */
	result = pff_test_open_file_io_handle(
	          &file_io_handle,
	          data,
	          128,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	PFF_TEST_ASSERT_IS_NOT_NULL(
	 "file_io_handle",
	 file_io_handle );

	PFF_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	/* Test regular cases
	 */

	/* Test error cases
	 */
	result = libpff_local_descriptors_tree_get_leaf_node_by_identifier(
	          NULL,
	          NULL,
	          NULL,
	          0,
	          NULL,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 -1 );

	PFF_TEST_ASSERT_IS_NOT_NULL(
	 "error",
	 error );

	libcerror_error_free(
	 &error );

	/* Clean up file IO handle
	 */
	result = pff_test_close_file_io_handle(
	          &file_io_handle,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 0 );

	PFF_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	/* Clean up
	 */
	result = libpff_local_descriptors_tree_free(
	          &local_descriptors_tree,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	PFF_TEST_ASSERT_IS_NULL(
	 "local_descriptors_tree",
	 local_descriptors_tree );

	PFF_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	result = libpff_offsets_index_free(
	          &offsets_index,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	PFF_TEST_ASSERT_IS_NULL(
	 "offsets_index",
	 offsets_index );

	PFF_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	result = libpff_io_handle_free(
	          &io_handle,
	          &error );

	PFF_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	PFF_TEST_ASSERT_IS_NULL(
	 "io_handle",
	 io_handle );

	PFF_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	return( 1 );

on_error:
	if( error != NULL )
	{
		libcerror_error_free(
		 &error );
	}
	if( file_io_handle != NULL )
	{
		libbfio_handle_free(
		 &file_io_handle,
		 NULL );
	}
	if( local_descriptors_tree != NULL )
	{
		libpff_local_descriptors_tree_free(
		 &local_descriptors_tree,
		 NULL );
	}
	if( offsets_index != NULL )
	{
		libpff_offsets_index_free(
		 &offsets_index,
		 NULL );
	}
	if( io_handle != NULL )
	{
		libpff_io_handle_free(
		 &io_handle,
		 NULL );
	}
	return( 0 );
}

#endif /* defined( __GNUC__ ) && !defined( LIBPFF_DLL_IMPORT ) */

/* The main program
 */
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
int wmain(
     int argc PFF_TEST_ATTRIBUTE_UNUSED,
     wchar_t * const argv[] PFF_TEST_ATTRIBUTE_UNUSED )
#else
int main(
     int argc PFF_TEST_ATTRIBUTE_UNUSED,
     char * const argv[] PFF_TEST_ATTRIBUTE_UNUSED )
#endif
{
	PFF_TEST_UNREFERENCED_PARAMETER( argc )
	PFF_TEST_UNREFERENCED_PARAMETER( argv )

#if defined( __GNUC__ ) && !defined( LIBPFF_DLL_IMPORT )

	PFF_TEST_RUN(
	 "libpff_local_descriptors_tree_initialize",
	 pff_test_local_descriptors_tree_initialize );

	PFF_TEST_RUN(
	 "libpff_local_descriptors_tree_free",
	 pff_test_local_descriptors_tree_free );

	/* TODO: add tests for libpff_local_descriptors_tree_clone */

	PFF_TEST_RUN(
	 "libpff_local_descriptors_tree_get_leaf_node_by_identifier",
	 pff_test_local_descriptors_tree_get_leaf_node_by_identifier );

	/* TODO: add tests for libpff_local_descriptors_tree_node_get_leaf_node_by_identifier */

	/* TODO: add tests for libpff_local_descriptors_tree_get_value_by_identifier */

	/* TODO: add tests for libpff_local_descriptors_tree_read */

#endif /* defined( __GNUC__ ) && !defined( LIBPFF_DLL_IMPORT ) */

	return( EXIT_SUCCESS );

#if defined( __GNUC__ ) && !defined( LIBPFF_DLL_IMPORT )

on_error:
	return( EXIT_FAILURE );

#endif /* defined( __GNUC__ ) && !defined( LIBPFF_DLL_IMPORT ) */
}

