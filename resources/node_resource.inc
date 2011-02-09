<?php
// $Id$

function _node_resource_definition() {
  return array(
    'node' => array(
      'retrieve' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/node_resource'),
        'callback' => '_node_resource_retrieve',
        'args' => array(
          array(
            'name' => 'nid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
            'description' => 'The nid of the node to get',
          ),
        ),
        'access callback' => '_node_resource_access',
        'access arguments' => array('view'),
        'access arguments append' => TRUE,
      ),
      'create' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/node_resource'),
        'callback' => '_node_resource_create',
        'args' => array(
          array(
            'name' => 'node',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The node object to create',
            'type' => 'struct',
          ),
        ),
        'access callback' => '_node_resource_access',
        'access arguments' => array('create'),
        'access arguments append' => TRUE,
      ),
      'update' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/node_resource'),
        'callback' => '_node_resource_update',
        'args' => array(
          array(
            'name' => 'nid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'int',
            'description' => 'The nid of the node to get',
          ),
          array(
            'name' => 'node',
            'optional' => FALSE,
            'source' => 'data',
            'description' => 'The node data to update',
            'type' => 'struct',
          ),
        ),
        'access callback' => '_node_resource_access',
        'access arguments' => array('update'),
        'access arguments append' => TRUE,
      ),
      'delete' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/node_resource'),
        'callback' => '_node_resource_delete',
        'args' => array(
          array(
            'name' => 'nid',
            'optional' => FALSE,
            'type' => 'int',
            'source' => array('path' => 0),
          ),
        ),
        'access callback' => '_node_resource_access',
        'access arguments' => array('delete'),
        'access arguments append' => TRUE,
      ),
      'index' => array(
        'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/node_resource'),
        'callback' => '_node_resource_index',
        'args' => array(
          array(
            'name' => 'page',
            'optional' => TRUE,
            'type' => 'int',
            'description' => 'The zero-based index of the page to get, defaults to 0.',
            'default value' => 0,
            'source' => array('param' => 'page'),
          ),
          array(
            'name' => 'fields',
            'optional' => TRUE,
            'type' => 'string',
            'description' => 'The fields to get.',
            'default value' => '*',
            'source' => array('param' => 'fields'),
          ),
          array(
            'name' => 'parameters',
            'optional' => TRUE,
            'type' => 'struct',
            'description' => 'Parameters',
            'default value' => NULL,
            'source' => array('param' => 'parameters'),
          ),
        ),
        'access arguments' => array('access content'),
      ),
    ),
  );
}

/**
 * Returns the results of a node_load() for the specified node.
 *
 * This returned node may optionally take content_permissions settings into
 * account, based on a configuration setting. 
 *
 * @param $nid
 *   NID of the node we want to return.
 * @return
 *   Node object or FALSE if not found.
 *
 * @see node_load()
 **/
function _node_resource_retrieve($nid) {
  global $user;
  $node = node_load($nid);

  if ($node) {
    // Apply field level content permissions
    if (module_exists('content') && variable_get('services_use_content_permissions', TRUE)) {
      $fields = content_fields(NULL, $node->type);
      foreach ($fields as $field_name => $field_info) {
        if (isset($node->$field_name)) {
          $access = module_invoke_all('field_access', 'view', $field_info, $user, $node);
          if (in_array(FALSE, $access)) {
            unset($node->$field_name);
          }
        }
      }
    }
    $node->uri = services_resource_uri(array('node', $node->nid));
    return $node;
  } else {
    return services_error('Node nid '. $nid .' not found', 404);
  }
}

/**
 * Creates a new node based on submitted values.
 *
 * Note that this function uses drupal_execute() to create new nodes,
 * which may require very specific formatting. The full implications of this
 * are beyond the scope of this comment block. The Googles are your friend.
 *
 * @param $node
 *   Array representing the attributes a node edit form would submit.
 * @return
 *   An associative array contained the new node's nid and, if applicable,
 *   the fully qualified URI to this resource.
 *
 * @see drupal_execute()
 **/
function _node_resource_create($node) {
  $node = (object)$node;

  if (!isset($node->type)) {
    return services_error('Missing node type', 406);
  }

  // Load the required includes for drupal_execute
  module_load_include('inc', 'node', 'node.pages');
  $nid = NULL;
  if (module_exists('content') && variable_get('services_use_content_permissions', TRUE)) {
    $fields = content_fields(NULL, $node->type);
    foreach ($fields as $field_name => $field_info) {
      if (isset($node->$field_name)) {
        $access = module_invoke_all('field_access', 'view', $field_info, $user, $node);
        if (in_array(FALSE, $access)) {
          unset($node->$field_name);
        }
      }
    }
  }
  // Setup form_state
  $values = (array)$node;
  $form_state = array();
  $form_state['values'] = $values;
  $form_state['values']['op'] = t('Save');
  $ret = drupal_execute($node->type . '_node_form', $form_state, $node);

  if ($errors = form_get_errors()) {
    return services_error(implode(" ", $errors), 406);
  }
  // Fetch $nid out of $form_state
  $nid = $form_state['nid'];
  // Only add the URI for servers that support it.
  $node = array('nid' => $nid);
  if ($uri = services_resource_uri(array('node', $nid))) {
    $node['uri'] = $uri;
  }
  return $node;
}

/**
 * Updates a new node based on submitted values.
 *
 * Note that this function uses drupal_execute() to create new nodes,
 * which may require very specific formatting. The full implications of this
 * are beyond the scope of this comment block. The Googles are your friend.
 *
 * @param $nid
 *   Node ID of the node we're editing.
 * @param $node
 *   Array representing the attributes a node edit form would submit.
 * @return
 *   The node's nid.
 *
 * @see drupal_execute()
 **/
function _node_resource_update($nid, $node) {
  $node = (object)$node;
  $node->nid = $nid;

  $old_node = node_load($node->nid);
  if ($old_node->nid) {
    // Load the required includes for drupal_execute
    module_load_include('inc', 'node', 'node.pages');

    // Setup form_state.
    $values = (array)$node;
    $form_state = array();
    $form_state['values'] = $values;
    $form_state['values']['op'] = t('Save');
    $form_state['node'] = (array)$old_node;

    drupal_execute($old_node->type . '_node_form', $form_state, $old_node);

    if ($errors = form_get_errors()) {
      return services_error(implode("\n", $errors), 406);
    }
  }
  else {
    return services_error(t('Node not found'), 404);
  }

  return $node->nid;
}

/**
 * Delete a node given its nid.
 *
 * @param $nid
 *   Node ID of the node we're deleting.
 * @return
 *   The node's nid.
 **/
function _node_resource_delete($nid) {
  node_delete($nid);
  return 1;
}

/**
 * Return an array of optionally paged nids baed on a set of criteria.
 *
 * An example request might look like
 *
 * http://domain/endpoint/node?fields=nid,vid&parameters[nid]=7&parameters[uid]=1
 *
 * This would return an array of objects with only nid and vid defined, where
 * nid = 7 and uid = 1.
 *
 * @param $page
 *   Page number of results to return (in pages of 20).
 * @param $fields
 *   The fields you want returned. 
 * @param $parameters
 *   An array of fields and values used to build a sql WHERE clause indicating
 *   what items should be deleted.
 * @return
 *   An array of node objects.
 *
 * @todo 
 *   Evaluate the functionality here in general. Particularly around
 *     - Do we need fields at all? Should this just return full nodes?
 *     - Is there an easier syntax we can define which can make the urls
 *       for index requests more straightforward?
 **/
function _node_resource_index($page = 0, $fields = '*', $parameters = array()) {
  $parameters = (array) $parameters;
  $schema = drupal_get_schema('node');
  $where = array();
  $fields = db_escape_string($fields);


  // Limit to published nodes if user doesn't have 'administer nodes'
  // permissions.
  if (!user_access('administer nodes')) {	
    $parameters['status'] = 1;
  }
  
  // Build an array of fields with the appropriate placeholders for use in
  // db_query().
  foreach ($parameters as $field => $value) {
    $where[] = $field . ' = ' . db_type_placeholder($schema['fields'][$field]['type']);
  }

  // Now implode that array into an actual WHERE clause.
  $where = !empty($where) ? ' WHERE '. implode(' AND ', $where) : '';

  // Run through db_rewrite_sql to make sure proper access checks are applied.
  $sql = "SELECT $fields FROM {node} n $where ORDER BY sticky DESC, created DESC";
  $sql = db_rewrite_sql($sql);
  $result = db_query_range($sql, $parameters, $page * 20, 20);

  // Put together array of matching nodes to return.
  $nodes = array();
  while ($node = db_fetch_object($result)) {
    if ($uri = services_resource_uri(array('node', $node->nid))) {
      $node->uri = $uri;
    }
    $nodes[] = $node;
  }

  return $nodes;
}

/**
 * Determine whether the current user can access a node resource.
 *
 * @param $op
 *   One of view, update, create, delete per node_access().
 * @param $args
 *   Resource arguments passed through from the original request.
 * @return bool
 *
 * @see node_access()
 **/
function _node_resource_access($op = 'view', $args = array()) {
  // Make sure we have an object or this all fails, some servers can
  // mess up the types.
  if (is_array($args[0])) {
    $args[0] = (object) $args[0];
  } else if(!is_array($args[0]) && !is_object($args[0])) {  //This is to determine if it is just a string
    $args[0] = (object)array('nid'=>$args[0]);
  }

  if ($op != 'create' && !empty($args)) {
    $node = node_load($args[0]->nid);
  }
  else if ($op == 'create') {
    $node = $args[0]->type;
    return node_access($op, $node);
  }
  if(isset($node->nid) && $node->nid) {
    return node_access($op, $node);
  } else {
    return services_error('Node id: '. $args[0]->nid .' could not be found', 404);
  }
}