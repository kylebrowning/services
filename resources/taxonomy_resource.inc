<?php
// $Id$
/**
 * @file
 *  Link general taxonomy functionalities to services module.
 */

function _taxonomy_resource_definition() {
  if (!module_exists('taxonomy')) {
    return array();
  }

  return array(
    'taxonomy_term' => array(
      'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
      'retrieve' => array(
        'callback' => '_taxonomy_term_resource_retrieve',
        'args' => array(
          array(
            'name' => 'tid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
            'description' => 'The tid of the taxonomy term to get',
          ),
        ),
        'access callback' => '_taxonomy_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
      ),
      'create' => array(
        'callback' => '_taxonomy_term_resource_create',
        'args' => array(
          array(
            'name' => 'term',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The taxonomy term object to create',
            'type' => 'struct',
          ),
        ),
        'access callback' => '_taxonomy_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
      ),
      'update' => array(
        'callback' => '_taxonomy_term_resource_update',
        'args' => array(
          array(
            'name' => 'term',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The taxonomy term data to update',
            'type' => 'struct',
          ),
        ),
        'access callback' => '_taxonomy_resource_access',
        'access arguments' => array('update'),
        'access arguments append' => TRUE,
      ),
      'delete' => array(
        'callback' => '_taxonomy_term_resource_delete',
        'args' => array(
          array(
            'name' => 'tid',
            'optional' => FALSE,
            'source' => array('path' => 0),
          ),
        ),
        'access callback' => '_taxonomy_resource_access',
        'access arguments' => array('delete'),
        'access arguments append' => TRUE,
      ),
      // TODO: term specific actions need to be implemented (see: taxonomy_service_get_tree / taxonomy_service_select_nodes)
    ),
    'taxonomy_vocabulary' => array(
      'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
      'retrieve' => array(
        'callback' => '_taxonomy_vocabulary_resource_retrieve',
        'args' => array(
          array(
            'name' => 'vid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
            'description' => 'The vid of the taxonomy vocabulary to get',
          ),
        ),
        'access callback' => '_taxonomy_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
      ),
      'create' => array(
        'callback' => '_taxonomy_vocabulary_resource_create',
        'args' => array(
          array(
            'name' => 'vocabulary',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The taxonomy vocabulary object to create',
            'type' => 'struct',
          ),
        ),
        'access callback' => '_taxonomy_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
      ),
      'update' => array(
        'callback' => '_taxonomy_vocabulary_resource_update',
        'args' => array(
          array(
            'name' => 'vocabulary',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The taxonomy vocabulary data to update',
            'type' => 'struct',
          ),
        ),
        'access callback' => '_taxonomy_resource_access',
        'access arguments' => array('update'),
        'access arguments append' => TRUE,
      ),
      'delete' => array(
        'callback' => '_taxonomy_vocabulary_resource_delete',
        'args' => array(
          array(
            'name' => 'vid',
            'optional' => FALSE,
            'source' => array('path' => 0),
          ),
        ),
        'access callback' => '_taxonomy_resource_access',
        'access arguments' => array('delete'),
        'access arguments append' => TRUE,
      ),
    ),
  );
}

/**
 * Services interface to _taxonomy_term_resource_retrieve().
 *
 * @see
 *
 */
function _taxonomy_term_resource_retrieve($tid = NULL){
  return taxonomy_get_term($tid);
}

/**
 * Services interface to _taxonomy_term_resource_create().
 *
 * @see
 *
 */
function _taxonomy_term_resource_create($term=NULL){
  if (!is_array($term)) {
    $term = (array) $term;
  }
  return taxonomy_save_term($term);
}

/**
 * Services interface to _taxonomy_term_resource_update().
 *
 * @see
 *
 */
function _taxonomy_term_resource_update($term=NULL){
  return _taxonomy_term_resource_create($term);
}

/**
 * Services interface to _taxonomy_term_resource_delete().
 *
 * @see
 *
 */
function _taxonomy_term_resource_delete($tid=NULL){
  return taxonomy_del_term($tid);
}


/**
 * Services interface to _taxonomy_vocabulary_resource_retrieve().
 *
 * @see
 *
 */
function _taxonomy_vocabulary_resource_retrieve($vid=NULL){
  return taxonomy_vocabulary_load($vid);
}

/**
 * Services interface to _taxonomy_vocabulary_resource_create().
 *
 * @see
 *
 */
function _taxonomy_vocabulary_resource_create($vocabulary){
  if (!is_array($vocabulary)) {
    $vocabulary = (array) $vocabulary;
  }
  return taxonomy_save_vocabulary($vocabulary);
}

/**
 * Services interface to _taxonomy_vocabulary_resource_update().
 *
 * @see
 *
 */
function _taxonomy_vocabulary_resource_update($vocabulary){
  return _taxonomy_vocabulary_resource_create($vocabulary);
}

/**
 * Services interface to _taxonomy_vocabulary_resource_delete().
 *
 * @see
 *
 */
function _taxonomy_vocabulary_resource_delete($vid=NULL){
  return taxonomy_del_vocabulary($vid);
}



/**
 * Services interface to taxonomy_get_tree().
 *
 * @see
 *   taxonomy_get_tree()
 */
function taxonomy_service_get_tree($vid, $parent = 0, $max_depth = NULL) {
  return taxonomy_get_tree($vid, $parent, -1, $max_depth);
}

/**
 * Services interface to taxonomy_select_nodes().
 *
 * Note that where taxonomy_select_nodes() returns the results
 * of a db_query(), this function returns an array of node objects.
 *
 * @see
 *   taxonomy_select_nodes().
 * @return 
 *   An array of node objects.
 */
function taxonomy_service_select_nodes($tids = array(), $fields = array(), $operator = 'or', $depth = 0, $pager = TRUE, $order = 'n.sticky DESC, n.created DESC') {
  $result = taxonomy_select_nodes($tids, $operator, $depth, $pager, $order);
  while ($node = db_fetch_object($result)) {
    $nodes[] = services_node_load(node_load($node->nid), $fields);
  }

  return $nodes;
}
