/*
 * Item tree functions
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
#include <memory.h>
#include <types.h>

#include "libpff_definitions.h"
#include "libpff_descriptors_index.h"
#include "libpff_index_tree.h"
#include "libpff_index_value.h"
#include "libpff_item_descriptor.h"
#include "libpff_libbfio.h"
#include "libpff_libcdata.h"
#include "libpff_libcerror.h"
#include "libpff_libcnotify.h"
#include "libpff_libfcache.h"
#include "libpff_libfdata.h"
#include "libpff_item_descriptor.h"
#include "libpff_item_tree.h"

/* Creates an item tree
 * Make sure the value item_tree is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libpff_item_tree_initialize(
     libpff_item_tree_t **item_tree,
     libcerror_error_t **error )
{
	static char *function = "libpff_item_tree_initialize";

	if( item_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item tree.",
		 function );

		return( -1 );
	}
	if( *item_tree != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid item tree value already set.",
		 function );

		return( -1 );
	}
	*item_tree = memory_allocate_structure(
	              libpff_item_tree_t );

	if( *item_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create item tree.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *item_tree,
	     0,
	     sizeof( libpff_item_tree_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear item tree.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *item_tree != NULL )
	{
		memory_free(
		 *item_tree );

		*item_tree = NULL;
	}
	return( -1 );
}

/* Frees an item tree
 * Returns 1 if successful or -1 on error
 */
int libpff_item_tree_free(
     libpff_item_tree_t **item_tree,
     libcerror_error_t **error )
{
	static char *function = "libpff_item_tree_free";
	int result            = 1;

	if( item_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item tree.",
		 function );

		return( -1 );
	}
	if( *item_tree != NULL )
	{
		if( ( *item_tree )->root_node != NULL )
		{
			if( libcdata_tree_node_free(
			     &( ( *item_tree )->root_node ),
			     (int (*)(intptr_t **, libcerror_error_t **)) &libpff_item_descriptor_free,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free root node.",
				 function );

				result = -1;
			}
		}
		memory_free(
		 *item_tree );

		*item_tree = NULL;
	}
	return( result );
}

/* Frees a recovered item tree node
 * Returns 1 if successful or -1 on error
 */
int libpff_item_tree_node_free_recovered(
     libcdata_tree_node_t **item_tree_node,
     libcerror_error_t **error )
{
	static char *function = "libpff_item_tree_node_free_recovered";
	int result            = 1;

	if( item_tree_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item tree node.",
		 function );

		return( -1 );
	}
	if( *item_tree_node != NULL )
	{
		if( libcdata_tree_node_free(
		     item_tree_node,
		     (int (*)(intptr_t **, libcerror_error_t **)) &libpff_item_descriptor_free,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free tree node.",
			 function );

			result = -1;
		}
	}
	return( result );
}

/* Retrieves the tree node of an item node
 * Returns 1 if successful, 0 if the item node was not found or -1 on error
 */
int libpff_item_tree_get_tree_node_by_identifier(
     libcdata_tree_node_t *item_tree_node,
     uint32_t item_identifier,
     libcdata_tree_node_t **result_item_tree_node,
     int recursion_depth,
     libcerror_error_t **error )
{
	libcdata_tree_node_t *sub_tree_node       = NULL;
	libpff_item_descriptor_t *item_descriptor = NULL;
	static char *function                     = "libpff_item_tree_get_tree_node_by_identifier";
	int number_of_sub_nodes                   = 0;
	int result                                = 0;
	int sub_node_index                        = 0;

	if( item_tree_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item tree node.",
		 function );

		return( -1 );
	}
	if( ( recursion_depth < 0 )
	 || ( recursion_depth > LIBPFF_MAXIMUM_ITEM_TREE_RECURSION_DEPTH ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid recursion depth value out of bounds.",
		 function );

		return( -1 );
	}
	if( libcdata_tree_node_get_value(
	     item_tree_node,
	     (intptr_t **) &item_descriptor,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve item descriptor.",
		 function );

		return( -1 );
	}
	if( item_descriptor == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing item descriptor.",
		 function );

		return( -1 );
	}
	if( item_descriptor->descriptor_identifier == item_identifier )
	{
		if( result_item_tree_node == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid result item tree node.",
			 function );

			return( -1 );
		}
		if( *result_item_tree_node != NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
			 "%s: result item tree node already set.",
			 function );

			return( -1 );
		}
		*result_item_tree_node = item_tree_node;

		return( 1 );
	}
	if( libcdata_tree_node_get_number_of_sub_nodes(
	     item_tree_node,
	     &number_of_sub_nodes,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of sub nodes.",
		 function );

		return( -1 );
	}
	if( number_of_sub_nodes > 0 )
	{
		if( libcdata_tree_node_get_sub_node_by_index(
		     item_tree_node,
		     0,
		     &sub_tree_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve first sub node.",
			 function );

			return( -1 );
		}
		for( sub_node_index = 0;
		     sub_node_index < number_of_sub_nodes;
		     sub_node_index++ )
		{
			if( sub_tree_node == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: corruption detected for sub node: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
			result = libpff_item_tree_get_tree_node_by_identifier(
			          sub_tree_node,
			          item_identifier,
			          result_item_tree_node,
			          recursion_depth + 1,
			          error );

			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to traverse sub node: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
			else if( result != 0 )
			{
				break;
			}
			if( libcdata_tree_node_get_next_node(
			     sub_tree_node,
			     &sub_tree_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve next node of sub node: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
		}
	}
	return( result );
}

/* Retrieves the sub node of an item node
 * Returns 1 if successful, 0 if the item node was not found or -1 on error
 */
int libpff_item_tree_get_sub_node_by_identifier(
     libcdata_tree_node_t *item_tree_node,
     uint32_t sub_node_identifier,
     libcdata_tree_node_t **sub_node,
     libcerror_error_t **error )
{
	libcdata_tree_node_t *sub_tree_node       = NULL;
	libpff_item_descriptor_t *item_descriptor = NULL;
	static char *function                     = "libpff_item_tree_get_sub_node_by_identifier";
	int number_of_sub_nodes                   = 0;
	int result                                = 0;
	int sub_node_index                        = 0;

	if( item_tree_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item tree node.",
		 function );

		return( -1 );
	}
	if( sub_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sub node.",
		 function );

		return( -1 );
	}
	if( *sub_node != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: sub node already set.",
		 function );

		return( -1 );
	}
	if( libcdata_tree_node_get_value(
	     item_tree_node,
	     (intptr_t **) &item_descriptor,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve item descriptor.",
		 function );

		return( -1 );
	}
	if( item_descriptor == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing item descriptor.",
		 function );

		return( -1 );
	}
	if( libcdata_tree_node_get_number_of_sub_nodes(
	     item_tree_node,
	     &number_of_sub_nodes,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of sub nodes.",
		 function );

		return( -1 );
	}
	if( number_of_sub_nodes > 0 )
	{
		if( libcdata_tree_node_get_sub_node_by_index(
		     item_tree_node,
		     0,
		     &sub_tree_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve first sub node.",
			 function );

			return( -1 );
		}
		for( sub_node_index = 0;
		     sub_node_index < number_of_sub_nodes;
		     sub_node_index++ )
		{
			if( libcdata_tree_node_get_value(
			     sub_tree_node,
			     (intptr_t **) &item_descriptor,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve item descriptor from sub node: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
			if( item_descriptor == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: missing sub item descriptor: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
			if( item_descriptor->descriptor_identifier == sub_node_identifier )
			{
				*sub_node = sub_tree_node;

				return( 1 );
			}
			if( libcdata_tree_node_get_next_node(
			     sub_tree_node,
			     &sub_tree_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve next node of sub node: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
		}
	}
	return( result );
}

/* Appends the identifier of the item
 * Returns 1 if successful or -1 on error
 */
int libpff_item_tree_append_identifier(
     libcdata_tree_node_t *item_tree_node,
     uint32_t descriptor_identifier,
     uint64_t data_identifier,
     uint64_t local_descriptors_identifier,
     uint8_t recovered,
     libcerror_error_t **error )
{
	libpff_item_descriptor_t *item_descriptor = NULL;
	static char *function                     = "libpff_item_tree_append_identifier";

	if( item_tree_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item tree node.",
		 function );

		return( -1 );
	}
	if( libpff_item_descriptor_initialize(
	     &item_descriptor,
	     descriptor_identifier,
	     data_identifier,
	     local_descriptors_identifier,
	     recovered,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create item descriptor.",
		 function );

		goto on_error;
	}
	if( item_descriptor == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing item descriptor.",
		 function );

		goto on_error;
	}
	if( libcdata_tree_node_append_value(
	     item_tree_node,
	     (intptr_t *) item_descriptor,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append item descriptor to item tree node.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( item_descriptor != NULL )
	{
		libpff_item_descriptor_free(
		 &item_descriptor,
		 NULL );
	}
	return( -1 );
}

/* Creates an item tree from the descriptors index
 * Returns 1 if successful or -1 on error
 */
int libpff_item_tree_create(
     libpff_item_tree_t *item_tree,
     libbfio_handle_t *file_io_handle,
     libpff_descriptors_index_t *descriptors_index,
     libcdata_list_t *orphan_node_list,
     libcdata_tree_node_t **root_folder_item_tree_node,
     libcerror_error_t **error )
{
	libfdata_tree_node_t *descriptor_index_tree_root_node = NULL;
	libpff_item_descriptor_t *item_descriptor             = NULL;
	static char *function                                 = "libpff_item_tree_create";

	if( item_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item tree.",
		 function );

		return( -1 );
	}
	if( item_tree->root_node != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid item tree - root node already set.",
		 function );

		return( -1 );
	}
	if( libpff_index_tree_get_root_node(
	     descriptors_index->index_tree,
	     &descriptor_index_tree_root_node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to retrieve descriptor index tree root node.",
		 function );

		goto on_error;
	}
	if( libpff_item_descriptor_initialize(
	     &item_descriptor,
	     0,
	     0,
	     0,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create item descriptor.",
		 function );

		goto on_error;
	}
	if( libcdata_tree_node_initialize(
	     &( item_tree->root_node ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create item tree root node.",
		 function );

		goto on_error;
	}
	if( libcdata_tree_node_set_value(
	     item_tree->root_node,
	     (intptr_t *) item_descriptor,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set item descriptor in item tree root node.",
		 function );

		goto on_error;
	}
	/* The item descriptor is now managed by the item tree root node
	 */
	item_descriptor = NULL;

	if( libpff_item_tree_create_node(
	     item_tree,
	     file_io_handle,
	     descriptors_index->index_tree,
	     descriptor_index_tree_root_node,
	     descriptors_index->index_cache,
	     orphan_node_list,
	     root_folder_item_tree_node,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create item tree.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( item_tree->root_node != NULL )
	{
		libcdata_tree_node_free(
		 &( item_tree->root_node ),
		 (int (*)(intptr_t **, libcerror_error_t **)) &libpff_item_descriptor_free,
		 NULL );
	}
	if( item_descriptor != NULL )
	{
		libpff_item_descriptor_free(
		 &item_descriptor,
		 NULL );
	}
	return( -1 );
}

/* Creates an item tree node from the descriptor index
 *
 * If a descriptor index value has no existing parent it is added to the orphan node list
 *
 * Returns 1 if successful or -1 on error
 */
int libpff_item_tree_create_node(
     libpff_item_tree_t *item_tree,
     libbfio_handle_t *file_io_handle,
     libpff_index_tree_t *descriptor_index_tree,
     libfdata_tree_node_t *descriptor_index_tree_node,
     libfcache_cache_t *index_tree_cache,
     libcdata_list_t *orphan_node_list,
     libcdata_tree_node_t **root_folder_item_tree_node,
     int recursion_depth,
     libcerror_error_t **error )
{
	libfdata_tree_node_t *descriptor_index_tree_sub_node = NULL;
	static char *function                                = "libpff_item_tree_create_node";
	int number_of_sub_nodes                              = 0;
	int result                                           = 0;
	int sub_node_index                                   = 0;

	if( item_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item tree.",
		 function );

		return( -1 );
	}
	if( root_folder_item_tree_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid root folder item tree node.",
		 function );

		return( -1 );
	}
	if( ( recursion_depth < 0 )
	 || ( recursion_depth > LIBPFF_MAXIMUM_ITEM_TREE_RECURSION_DEPTH ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid recursion depth value out of bounds.",
		 function );

		return( -1 );
	}
	/* Check if the index node can be read
	 */
	if( libfdata_tree_node_get_number_of_sub_nodes(
	     descriptor_index_tree_node,
	     (intptr_t *) file_io_handle,
	     (libfdata_cache_t *) index_tree_cache,
	     &number_of_sub_nodes,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of sub nodes from descriptor index tree node.",
		 function );

#if defined( HAVE_DEBUG_OUTPUT )
		if( ( libcnotify_verbose != 0 )
		 && ( error != NULL )
		 && ( *error != NULL ) )
		{
			libcnotify_print_error_backtrace(
			 *error );
		}
#endif
		libcerror_error_free(
		 error );

/* TODO flag corrupt item tree */

		return( 1 );
	}
	result = libfdata_tree_node_is_deleted(
	          descriptor_index_tree_node,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if descriptor index tree node is deleted.",
		 function );

		goto on_error;
	}
	else if( result != 0 )
	{
		return( 1 );
	}
	result = libfdata_tree_node_is_leaf(
	          descriptor_index_tree_node,
	          (intptr_t *) file_io_handle,
	          (libfdata_cache_t *) index_tree_cache,
	          0,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if descriptor index tree node is a leaf node.",
		 function );

		goto on_error;
	}
	else if( result != 0 )
	{
		if( libpff_item_tree_create_leaf_node(
		     item_tree,
		     file_io_handle,
		     descriptor_index_tree,
		     descriptor_index_tree_node,
		     index_tree_cache,
		     orphan_node_list,
		     root_folder_item_tree_node,
		     recursion_depth,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create index tree from descriptor index tree leaf node.",
			 function );

			goto on_error;
		}
	}
	else
	{
		for( sub_node_index = 0;
		     sub_node_index < number_of_sub_nodes;
		     sub_node_index++ )
		{
			if( libfdata_tree_node_get_sub_node_by_index(
			     descriptor_index_tree_node,
			     (intptr_t *) file_io_handle,
			     (libfdata_cache_t *) index_tree_cache,
			     sub_node_index,
			     &descriptor_index_tree_sub_node,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve sub node: %d from descriptor index tree node.",
				 function,
				 sub_node_index );

				goto on_error;
			}
			if( libpff_item_tree_create_node(
			     item_tree,
			     file_io_handle,
			     descriptor_index_tree,
			     descriptor_index_tree_sub_node,
			     index_tree_cache,
			     orphan_node_list,
			     root_folder_item_tree_node,
			     recursion_depth + 1,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
				 "%s: unable to create index tree from descriptor index tree sub node: %d.",
				 function,
				 sub_node_index );

				goto on_error;
			}
		}
	}
	return( 1 );

on_error:
	if( *root_folder_item_tree_node != NULL )
	{
		libcdata_tree_node_free(
		 root_folder_item_tree_node,
		 NULL,
		 NULL );
	}
	return( -1 );
}

/* Creates an item tree leaf node from the descriptor index
 *
 * If a descriptor index value has no existing parent it is added to the orphan node list
 *
 * Returns 1 if successful or -1 on error
 */
int libpff_item_tree_create_leaf_node(
     libpff_item_tree_t *item_tree,
     libbfio_handle_t *file_io_handle,
     libpff_index_tree_t *descriptor_index_tree,
     libfdata_tree_node_t *descriptor_index_tree_node,
     libfcache_cache_t *index_tree_cache,
     libcdata_list_t *orphan_node_list,
     libcdata_tree_node_t **root_folder_item_tree_node,
     int recursion_depth,
     libcerror_error_t **error )
{
	libcdata_tree_node_t *item_tree_node                    = NULL;
	libcdata_tree_node_t *parent_node                       = NULL;
	libfdata_tree_node_t *descriptor_index_tree_parent_node = NULL;
	libpff_index_value_t *descriptor_index_value            = NULL;
	libpff_item_descriptor_t *item_descriptor               = NULL;
	static char *function                                   = "libpff_item_tree_create_leaf_node";
	uint32_t identifier                                     = 0;
	uint32_t parent_identifier                              = 0;
	int leaf_node_index                                     = 0;
	int result                                              = 0;

	if( item_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item tree.",
		 function );

		return( -1 );
	}
	if( root_folder_item_tree_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid root folder item tree node.",
		 function );

		return( -1 );
	}
	if( ( recursion_depth < 0 )
	 || ( recursion_depth > LIBPFF_MAXIMUM_ITEM_TREE_RECURSION_DEPTH ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid recursion depth value out of bounds.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_node_value(
	     descriptor_index_tree_node,
	     (intptr_t *) file_io_handle,
	     (libfdata_cache_t *) index_tree_cache,
	     (intptr_t **) &descriptor_index_value,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve descriptor index tree sub node value.",
		 function );

		goto on_error;
	}
	if( descriptor_index_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing descriptor index tree sub node value.",
		 function );

		goto on_error;
	}
	if( descriptor_index_value->identifier > (uint64_t) UINT32_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: descriptor index identifier value exceeds maximum.",
		 function );

		goto on_error;
	}
	/* Create a new item descriptor
	 */
	if( libpff_item_descriptor_initialize(
	     &item_descriptor,
	     (uint32_t) descriptor_index_value->identifier,
	     descriptor_index_value->data_identifier,
	     descriptor_index_value->local_descriptors_identifier,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create item descriptor.",
		 function );

		goto on_error;
	}
	/* The descriptor_index_value can be cached out by the next call to
	 * libpff_item_tree_create_leaf_node so preserve values that are needed.
	 */
	identifier        = (uint32_t) descriptor_index_value->identifier;
	parent_identifier = descriptor_index_value->parent_identifier;

	descriptor_index_value = NULL;

	/* The root folder index descriptor points to itself as its parent
	 */
	if( identifier == parent_identifier )
	{
		if( *root_folder_item_tree_node != NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
			 "%s: root folder item tree node already set.",
			 function );

			goto on_error;
		}
		if( libcdata_tree_node_initialize(
		     root_folder_item_tree_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create root folder item tree node.",
			 function );

			goto on_error;
		}
		if( libcdata_tree_node_set_value(
		     *root_folder_item_tree_node,
		     (intptr_t *) item_descriptor,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set item descriptor in root folder item tree node.",
			 function );

			goto on_error;
		}
		result = libcdata_tree_node_insert_node(
			  item_tree->root_node,
			  *root_folder_item_tree_node,
			  (int (*)(intptr_t *, intptr_t *, libcerror_error_t **)) &libpff_item_descriptor_compare,
			  LIBCDATA_INSERT_FLAG_UNIQUE_ENTRIES,
			  NULL );

		/* Free the node if it could not be inserted
		 */
		if( result != 1 )
		{
			libcdata_tree_node_free(
			 root_folder_item_tree_node,
			 (int (*)(intptr_t **, libcerror_error_t **)) &libpff_item_descriptor_free,
			 NULL );
		}
		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
			 "%s: unable to insert item descriptor in item tree node.",
			 function );

			goto on_error;
		}
	}
	else
	{
		result = libpff_item_tree_get_tree_node_by_identifier(
			  item_tree->root_node,
			  parent_identifier,
			  &parent_node,
			  0,
			  error );

		if( result == 0 )
		{
#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				 "%s: reading ahead for descriptor: %" PRIu64 " parent %" PRIu32 ".\n",
				 function,
				 identifier,
				 parent_identifier );
			}
#endif
			result = libpff_index_tree_get_leaf_node_by_identifier(
				  descriptor_index_tree,
				  file_io_handle,
				  index_tree_cache,
				  parent_identifier,
				  &leaf_node_index,
				  &descriptor_index_tree_parent_node,
				  error );

			if( result == 1 )
			{
				if( descriptor_index_tree_parent_node == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing descriptor index tree parent node value.",
					 function );

					goto on_error;
				}
				if( libpff_item_tree_create_node(
				     item_tree,
				     file_io_handle,
				     descriptor_index_tree,
				     descriptor_index_tree_parent_node,
				     index_tree_cache,
				     orphan_node_list,
				     root_folder_item_tree_node,
				     recursion_depth + 1,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
					 "%s: unable to create index tree from descriptor index tree parent node: %" PRIu32 ".",
					 function,
					 parent_identifier );

					goto on_error;
				}
				parent_node = NULL;

				result = libpff_item_tree_get_tree_node_by_identifier(
					  item_tree->root_node,
					  parent_identifier,
					  &parent_node,
					  0,
					  error );
			}
		}
		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to find parent node: %" PRIu32 ".",
			 function,
			 parent_identifier );

			goto on_error;
		}
		else if( result == 0 )
		{
#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				 "%s: parent node: %" PRIu32 " missing - found orphan node: %" PRIu64 ".\n",
				 function,
				 parent_identifier,
				 identifier );
			}
#endif
			if( libcdata_tree_node_initialize(
			     &item_tree_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
				 "%s: unable to create item tree node.",
				 function );

				goto on_error;
			}
			if( libcdata_tree_node_set_value(
			     item_tree_node,
			     (intptr_t *) item_descriptor,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set item descriptor in item tree root node.",
				 function );

				goto on_error;
			}
			item_descriptor = NULL;

			if( libcdata_list_append_value(
			     orphan_node_list,
			     (intptr_t *) item_tree_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
				 "%s: unable to append orphan node in orphan node list.",
				 function );

				goto on_error;
			}
		}
		else
		{
			if( parent_node == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: invalid parent node.",
				 function );

				goto on_error;
			}
			result = libcdata_tree_node_insert_value(
			          parent_node,
			          (intptr_t *) item_descriptor,
			          (int (*)(intptr_t *, intptr_t *, libcerror_error_t **)) &libpff_item_descriptor_compare,
			          LIBCDATA_INSERT_FLAG_UNIQUE_ENTRIES,
			          error );

			/* Free the node if it could not be inserted
			 */
			if( result != 1 )
			{
				libpff_item_descriptor_free(
				 &item_descriptor,
				 NULL );
			}
			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
				 "%s: unable to insert item descriptor in item tree node.",
				 function );

				goto on_error;
			}
		}
	}
	return( 1 );

on_error:
	if( item_tree_node != NULL )
	{
		libcdata_tree_node_free(
		 &item_tree_node,
		 NULL,
		 NULL );
	}
	if( *root_folder_item_tree_node != NULL )
	{
		libcdata_tree_node_free(
		 root_folder_item_tree_node,
		 NULL,
		 NULL );
	}
	if( item_descriptor != NULL )
	{
		libpff_item_descriptor_free(
		 &item_descriptor,
		 NULL );
	}
	return( -1 );
}

/* Retrieves the tree node of an item node
 * Returns 1 if successful, 0 if the item node was not found or -1 on error
 */
int libpff_item_tree_get_node_by_identifier(
     libpff_item_tree_t *item_tree,
     uint32_t item_identifier,
     libcdata_tree_node_t **item_tree_node,
     libcerror_error_t **error )
{
	static char *function = "libpff_item_tree_get_node_by_identifier";
	int result            = 0;

	if( item_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item tree.",
		 function );

		return( -1 );
	}
	result = libpff_item_tree_get_tree_node_by_identifier(
	          item_tree->root_node,
	          item_identifier,
                  item_tree_node,
                  0,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve item tree node: %" PRIu32 ".",
		 function,
		 item_identifier );

		return( -1 );
	}
	return( result );
}

