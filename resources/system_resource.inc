<?php
// $Id$

/**
 * @file
 *  Link general system functionalities to services module.
 */

function _system_resource_definition() {
  return array(
    'system' => array(
      'actions' => array(
        'connect' => array(
          'access callback' => 'services_access_menu',
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/system_resource'),
          'callback' => '_system_resource_connect',
        ),
        'get_variable' => array(
          'help'   => t('Returns the value of a system variable using variable_get().'),
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/system_resource'),
          'callback' => '_system_resource_get_variable',
          'access arguments' => array('get a system variable'),
          'access arguments append' => FALSE,
          'args' => array(
            array(
              'name' => 'name',
              'optional' => FALSE,
              'source' => 'data',
              'description' => t('The name of the variable to return.'),
              'type' => 'string',
            ),
            array(
              'name' => 'default',
              'optional' => FALSE,
              'source' => 'data',
              'description' => t('The default value to use if this variable has never been set.'),
              'type' => 'string',
            ),
          ),
        ),
        'set_variable' => array(
          'help'   => t('Sets the value of a system variable using variable_set().'),
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/system_resource'),
          'callback' => '_system_resource_set_variable',
          'access arguments' => array('set a system variable'),
          'access arguments append' => FALSE,
          'args' => array(
            array(
              'name' => 'name',
              'optional' => FALSE,
              'source' => 'data',
              'description' => t('The name of the variable to set.'),
              'type' => 'string',
            ),
            array(
              'name' => 'value',
              'optional' => FALSE,
              'source' => 'data',
              'description' => t('The value to set.'),
              'type' => 'string',
            ),
          ),
        ),
      ),
    ),
  );
}

/**
 * Return the details of a new anonymous session.
 *
 * @return
 *   An object with an active anonymous session name and id.
 */
function _system_resource_connect() {
  global $user;

  $return = new stdClass();
  $return->sessid = session_id();
  $return->user = $user;

  return $return;
}

/**
 * Services implementation of variable_get().
 *
 * @param $name
 *   The name of the variable to return.
 * @param $default
 *   The value to use if the variable has never been set.
 *
 * @return
 *   The value of the variable.
 *
 * @see variable_get()
 */
function _system_resource_get_variable($name, $default) {
  return variable_get($name, $default);
}

/**
 * Services implementation of variable_set().
 *
 * @param $name
 *   The name of the variable to set.
 * @param $value
 *   The value to set this variable to.
 *
 * @see variable_set()
 */
function _system_service_set_variable($name, $value) {
  variable_set($name, $value);
}
