<?php
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
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
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
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
      ),
      'update' => array(
        'callback' => '_taxonomy_term_resource_update',
        'args' => array(
          array(
            'name' => 'tid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'description' => 'The unique identifier for this taxonomy term.',
            'type' => 'int',
          ),
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
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
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
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
      ),
      'actions' => array(
        'selectNodes' => array(
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
          'help'   => t('This method returns all nodes with provided taxonomy id.'),
          'access callback' => '_taxonomy_resource_access',
          'access arguments' => array('view'),
          'access arguments append' => TRUE,
          'callback' => 'taxonomy_service_select_nodes',
          'args'     => array(
            array(
              'name'         => 'tids',
              'type'         => 'string',
              'description'  => t('The vocabulary ids to retrieve, separated by comma.'),
              'source'       => 'data',
              'optional'     => FALSE,
            ),
            array(
              'name'          => 'fields',
              'type'          => 'string',
              'description'   => t('The fields to retrieve, separated by comma.'),
              'source'        => 'data',
              'optional'      => TRUE,
              'default value' => array(),
            ),
            array(
              'name'          => 'operator',
              'type'          => 'string',
              'description'   => t('How to interpret multiple IDs in the array. Can be "or" or "and".'),
              'source'        => 'data',
              'optional'      => TRUE,
              'default value' => 'or',
            ),
            array(
              'name'          => 'depth',
              'type'          => 'int',
              'description'   => t('How many levels deep to traverse the taxonomy tree. Can be a nonnegative integer or "all".'),
              'source'        => 'data',
              'optional'      => TRUE,
              'default value' => 0,
            ),
            array(
              'name'          => 'pager',
              'type'          => 'int',
              'description'   => t('Whether the nodes are to be used with a pager (the case on most Drupal pages) or not (in an XML feed, for example).'),
              'source'        => 'data',
              'optional'      => TRUE,
              'default value' => TRUE,
            ),
            array(
              'name'          => 'order',
              'type'          => 'int',
              'description'   => t('The order clause for the query that retrieve the nodes.'),
              'source'        => 'data',
              'optional'      => TRUE,
              'default value' => 'n.sticky DESC, n.created DESC',
            ),
          ),
        ),
      ),
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
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
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
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
      ),
      'update' => array(
        'callback' => '_taxonomy_vocabulary_resource_update',
        'args' => array(
          array(
            'name' => 'vid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'description' => 'The unique identifier for this taxonomy vocabulary.',
            'type' => 'int',
          ),
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
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
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
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
      ),
      'actions' => array(
        'getTree' => array(
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/taxonomy_resource'),
          'help'   => t('This method returns a full list of taxonomy terms.'),
          'access callback' => '_taxonomy_resource_access',
          'access arguments' => array('view'),
          'access arguments append' => TRUE,
          'callback' => 'taxonomy_service_get_tree',
          'args'     => array(
            array(
              'name'         => 'vid',
              'type'         => 'int',
              'description'  => t('The vocabulary id to retrieve.'),
              'source'       => 'data',
              'optional'     => FALSE,
            ),
            array(
              'name'         => 'parent',
              'type'         => 'int',
              'description'  => t('The term ID under which to generate the tree. If 0, generate the tree for the entire vocabulary.'),
              'source'       => 'data',
              'default value'      => 0,
              'optional'     => TRUE,
            ),
            array(
              'name'         => 'maxdepth',
              'type'         => 'int',
              'description'  => t('The number of levels of the tree to return. Leave NULL to return all levels.'),
              'source'       => 'data',
              'optional'     => TRUE,
              'default value'      => NULL,
            ),
          ),
        ),
      ),
    ),
  );
}

/**
 * Return the results of taxonomy_get_term() for a specified term id.
 *
 * @param $tid
 *   Unique identifier for the taxonomy term to retrieve.
 * @return
 *   A term object.
 *
 * @see taxonomy_get_term()
 */
function _taxonomy_term_resource_retrieve($tid) {
  return taxonomy_get_term($tid);
}

/**
 * Create a new taxonomy term based on submitted values.
 *
 * @param $term
 *   Array of values for the taxonomy term.
 * @return
 *   Status constant indicating if term was inserted or updated.
 *
 * @see taxonomy_save_term()
 */
function _taxonomy_term_resource_create($term) {
  $term = (array) $term;
  return taxonomy_save_term($term);
}

/**
 * Update a taxonomy term based on submitted values.
 *
 * @param $tid
 *   Unique identifier for the taxonomy term to update.
 * @param $term
 *   Array of values for the taxonomy term.
 * @return
 *   Status constant indicating if term was inserted or updated.
 *
 * @see taxonomy_save_term()
 */
function _taxonomy_term_resource_update($tid, $term) {
  $term = (array) $term;
  $term['tid'] = $tid;
  return _taxonomy_term_resource_create($term);
}

/**
 * Delete a taxonomy term.
 *
 * @param $tid
 *   Unique identifier for the taxonomy term to delete.
 * @return
 *   Status constant indicating deletion.
 *
 * @see taxonomy_del_term()
 */
function _taxonomy_term_resource_delete($tid) {
  return taxonomy_del_term($tid);
}


/**
 * Return the results of taxonomy_vocabulary_load() for a specified vocabulary id.
 *
 * @param $vid
 *   Unique identifier for the taxonomy term to retrieve.
 * @return
 *   A vocabulary object or FALSE.
 *
 * @see taxonomy_vocabulary_load()
 */
function _taxonomy_vocabulary_resource_retrieve($vid) {
  return taxonomy_vocabulary_load($vid);
}

/**
 * Create a new taxonomy vocabulary based on submitted values.
 *
 * Here is a sample vocabulary array, taken from
 * http://drupaldeveloper.in/article/programmatically-create-vocabulary
 *
 * @code
 * $vocabulary = array(
 *   'name'  => t("Name"). // Human readable name of the vocabulary
 *   'description' => t("Description"), // extended description of the vocabulary
 *   'help' => t("help"), // help text
 *   'tags' => 0, // 1 to make this vocabulary free tagging
 *   'multiple' => 0, // 1 to allow multiple selection
 *   'required' => 0, // 1 to make the terms mandatory to be selected
 *   'hierarchy' => 0, // 1 to allow and create hierarchy of the terms within the vocabulary
 *   'relations' => 0, // 1 to set and allow relation amongst multiple terms
 *   'module' => 'mymodule', // provide the module name in which the vocabulary is defined and which is calling this function
 *   'node' => array('story' => 1), // content types to which this vocabulary will be attached to
 *   'weight' => -9, // set the weight to display the vocabulary in the list
 * );
 * @endcode
 *
 * @param $vocabulary
 *   Array of values for the taxonomy vocabulary.
 * @return
 *   Status constant indicating if vocabulary was inserted or updated.
 *
 * @see taxonomy_save_vocabulary()
 */
function _taxonomy_vocabulary_resource_create($vocabulary) {
  $vocabulary = (array) $vocabulary;
  return taxonomy_save_vocabulary($vocabulary);
}

/**
 * Update a taxonomy vocabulary based on submitted values.
 *
 * @param $vid
 *   Unique identifier for the taxonomy term to retrieve.
 * @param $vocabulary
 *   Array of values for the taxonomy vocabulary.
 * @return
 *   Status constant indicating if vocabulary was inserted or updated.
 *
 * @see taxonomy_save_vocabulary()
 */
function _taxonomy_vocabulary_resource_update($vid, $vocabulary) {
  $vocabulary = (array) $vocabulary;
  $vocabulary['vid'] = $vid;
  return _taxonomy_vocabulary_resource_create($vocabulary);
}

/**
 * Delete a taxonomy vocabulary.
 *
 * @param $vid
 *   Unique identifier for the taxonomy term to retrieve.
 * @return
 *   Status constant indicating deletion.
 *
 * @see taxonomy_del_vocabulary()
 */
function _taxonomy_vocabulary_resource_delete($vid) {
  return taxonomy_del_vocabulary($vid);
}



/**
 * Services interface to taxonomy_get_tree().
 *
 * @see taxonomy_get_tree()
 */
function taxonomy_service_get_tree($vid, $parent = 0, $max_depth = NULL) {
  $terms = taxonomy_get_tree($vid, $parent, -1, $max_depth);
  if (empty($terms)) {
    return services_error('No vocabulary with id: '. $vid .' found.', 404);
  }
  return $terms;
}

/**
 * Services interface to taxonomy_select_nodes().
 *
 * Note that where taxonomy_select_nodes() returns the results
 * of a db_query(), this function returns an array of node objects.
 *
 * @see taxonomy_select_nodes()
 * @return
 *   An array of node objects.
 */
function taxonomy_service_select_nodes($tids, $fields, $operator, $depth, $pager, $order) {
  if (!is_array($tids)) {
    $tids =  explode(',', $tids);
  }
  if (!is_array($fields)) {
    $fields =  explode(',', $fields);
  }
  $result = taxonomy_select_nodes($tids, $operator, $depth, $pager, $order);
  while ($node = db_fetch_object($result)) {
    $nodes[] = services_node_load(node_load($node->nid), $fields);
  }
  if (empty($nodes)) {
    return services_error('No nodes were found with tid(s): '. implode(",", $tids), 404);
  }
  return $nodes;
}

function _taxonomy_resource_access($op = 'view', $args = array()) {
  if (user_access('administer taxonomy')) {
    return TRUE;
  }
  // TODO - check perms of user to return ability to interact with terms / vocabulary
}

