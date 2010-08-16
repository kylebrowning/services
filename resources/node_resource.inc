<?php
// $Id$

function _node_resource_definition() {
  return array(
    'node' => array(
      'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/node_resource'),
      'retrieve' => array(
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
        'callback' => '_node_resource_update',
        'args' => array(
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
        'callback' => '_node_resource_delete',
        'args' => array(
          array(
            'name' => 'nid',
            'optional' => FALSE,
            'source' => array('path' => 0),
          ),
        ),
        'access callback' => '_node_resource_access',
        'access arguments' => array('delete'),
        'access arguments append' => TRUE,
      ),
      'index' => array(
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
            'default value' => '',
            'source' => array('param' => 'fields'),
          ),
          array(
            'name' => 'parameters',
            'optional' => TRUE,
            'type' => 'struct',
            'description' => 'Parameters',
            'default value' => NULL,
            'source' => 'param',
          ),
        ),
        'access arguments' => array('access content'),
        'access arguments append' => TRUE,
      ),
    ),
  );
}

function _node_resource_retrieve($nid) {
  global $user;
  $node = node_load($nid);

  if ($node) {
    // Apply field level content permissions
    if (module_exists('content')) {
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
  }
  return $node;
}

function _node_resource_create($node) {
  $node = (object)$node;

  if (!isset($node->type)) {
    return services_error('Missing node type', 406);
  }

  // Load the required includes for drupal_execute
  module_load_include('inc', 'node', 'node.pages');
  $nid = NULL;

  // Setup form_state
  $values = (array)$node;
  $form_state = array();
  $form_state['values'] = $values;
  $form_state['values']['op'] = t('Save');
  $ret = drupal_execute($node->type . '_node_form', $form_state, $node);

  // Fetch $nid out of $form_state
  $nid = $form_state['nid'];

  if ($errors = form_get_errors()) {
    return services_error(implode("\n", $errors), 406);
  }

  return (object)array(
    'nid' => $nid,
    'uri' => services_resource_uri(array('node', $nid)),
  );
}

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

function _node_resource_delete($nid) {
  node_delete($nid);
  return TRUE;
}

function _node_resource_index($page = 0, $fields = array(), $parameters = array()) {
  $params = array();

  if (module_exists('query_builder')) {
    $builder = new QueryBuilder();
    $parameters = (array)$parameters;

    if (isset($parameters['__describe'])) {
      return $builder->describe();
    }

    if (is_string($fields) && !empty($fields)) {
      $fields = preg_split('/,\s?/', $fields);
    }

    if (!user_access('administer nodes') || !isset($parameters['status'])) {
      $parameters['status'] = 1;
    }
    list($sql, $params) = $builder->query($fields, $parameters);
  }
  else {
    $sql = "SELECT * FROM {node} WHERE status = 1 ORDER BY sticky DESC, created DESC";
  }
  $res = db_query_range($sql, $params, $page * 20, 20);

  $events = array();
  while ($event = db_fetch_object($res)) {
    $event->uri = services_resource_uri(array('node', $event->nid));
    $events[] = $event;
  }
  return $events;
}