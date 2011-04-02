<?php

/**
 * @file
 * Hooks provided by Services for the definition of new services.
 */

/**
 * @addtogroup hooks
 */

 /**
  * Defines function signatures for resources available to services.
  *
  * Functionally this is very similar to the way hook_menu() works, and in many
  * ways Services can be seen as an abstraction layer on top of hook_menu().
  *
  * @return
  *   An associative array which defines available resources.
  *
  *   The associative array which defines services has six possible top
  *   level keys:
  *
  *     - create
  *     - retrieve
  *     - update
  *     - delete
  *     - actions
  *     - targeted actions
  *
  *   The first four (the CRUD functions) define the indvidual service
  *   callbacks for each function. However 'actions' and 'targeted actions'
  *   can contain multiple callbacks.
  *
  *   For those familiar with Services 2.x, these callbacks are created
  *   similarly, but the keys have changed around a bit. The following keys
  *   are used to describe a callback.
  *
  *   - help: Text describing what this callback does.
  *   - callback: The name of a function to call when this resource is
  *     requested.
  *   - access callback: The name of a function to call to check whether
  *     the requesting user has permission to access this resource. If not
  *     specified, this defaults to 'user_access'.
  *   - access arguments: The arguments to pass to the access callback.
  *   - access arguments append: A boolean indicating whether the resource's
  *     arguments should be appended to the access arguments. This can be useful
  *     in situations where an access callback is specific to the particular
  *     item ('edit all nodes' vs 'edit my nodes'). Defaults to FALSE.
  *   - args: an array describing the arguments which should be passed to this
  *     resource when it is called. Each element in the array is an associative
  *     array containing the following keys:
  *
  *     - name: The name of this argument.
  *     - type: The data type of this argument (int, string, array)
  *     - description: Text describing this argument's usage.
  *     - optional: A boolean indicating whether or not this argument is optional.
  *     - source: Where this argument should be retrieved from. This can be
  *       'data' (indicating the POST data), 'param' (indicating the query
  *       string) or 'path' (indicating the url path). In the case of path,
  *       an additional parameter must be passed indicating the index to be used.
  *     - default value: this is a value that will be passed to the method for this particular argument if no argument value is passed
  */
function hook_services_resources() {
  return array(
    'user' => array(
      'file' => array('type' => 'inc', 'module' => 'user_resource'),
      'retrieve' => array(
        'help' => 'Retrieves a user',
        'callback' => '_user_resource_retrieve',
        'access arguments' => array('access user profiles'), // this is probably not enough, doesn't block things like pass and email
        'access arguments append' => TRUE,
        'args' => array(
          array(
            'name' => 'uid',
            'type' => 'int',
            'description' => 'The uid of the user to retrieve.',
            'source' => array('path' => '0'),
            'optional' => FALSE,
            'default value' => NULL,
          ),
        ),
      ),
    ),

    'actions' => array(
      'login' => array(
        'help' => 'Login a user for a new session',
        'callback' => '_user_resource_login',
        'args' => array(
          array(
            'name' => 'username',
            'type' => 'string',
            'description' => 'A valid username',
            'source' => array('data'),
            'optional' => FALSE,
          ),
          array(
            'name' => 'password',
            'type' => 'string',
            'description' => 'A valid password',
            'source' => array('data'),
            'optional' => FALSE,
          ),
        ),
      ),

      'logout' => array(
        'help' => 'Logout a user session',
        'callback' => '_user_resource_logout',
        'args' => array(
          array(
          ),
        ),
      ),
    ),
  );
}
