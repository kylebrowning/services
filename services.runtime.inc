<?php

/**
 * @file
 *  Contains functions that only are necessary when a service call is made.
 *  This has broken out so that this code isn't loaded for every page load.
 */

/**
 * A exception thrown by services and related modules when something goes
 * wrong.
 */
class ServicesException extends Exception {
  private $data;

  /**
   * Constructor for the ServicesException.
   *
   * @param string $message
   *  Error message.
   * @param int $code
   *  Optional. Error code. This often maps to the HTTP status codes. Defaults
   *  to 0.
   * @param mixed $data
   *  Information that can be used by the server to return information about
   *  the error.
   */
  public function __construct($message, $code = 0, $data = NULL) {
    parent::__construct($message, $code);

    $this->data = $data;
  }

  /**
   * Returns the data associated with the exception.
   *
   * @return mixed
   */
  public function getData() {
    return $this->data;
  }
}

/**
 * A exception thrown by services and related modules when an error related to
 * a specific argument is encountered.
 */
class ServicesArgumentException extends ServicesException {
  private $argument;

  /**
   * Constructor for the ServicesException.
   *
   * @param string $message
   *  Error message.
   * @param string $argument_name
   *  The name of the argument that caused the error.
   * @param int $code
   *  Optional. Error code. This often maps to the HTTP status codes. Defaults
   *  to 0.
   * @param mixed $data
   *  Information that can be used by the server to return information about
   *  the error.
   */
  public function __construct($message, $argument_name, $code, $data) {
    parent::__construct($message, $code, $data);

    $this->argument = $argument_name;
  }

  /**
   * Returns the name of the argument that caused the error.
   *
   * @return string
   *  The name of the argument.
   */
  public function getArgumentName() {
    return $this->argument;
  }
}

/**
 * Performs access checks and executes a services controller.
 * This method is called by server implementations.
 *
 * @param array $controller
 *  An array containing information about the controller
 * @param array $args
 *  The arguments that should be passed to the controller.
 * @param array $options
 *  Options for the execution. Use 'skip_authentication'=>TRUE to skip the
 *  services-specific authentication checks. Access checks will always be
 *  made.
 */
function services_controller_execute($controller, $args = array(), $options = array()) {
  global $user;

  $server_info =  services_server_info_object();
  if ($server_info->debug) {
    watchdog('services', 'Controller: <pre>@controller</pre>', array('@controller' => print_r($controller, TRUE)), WATCHDOG_DEBUG);
    watchdog('services', 'Passed arguments: <pre>@arguments</pre>', array('@arguments' => print_r($args, TRUE)), WATCHDOG_DEBUG);
  }

  $original_user = $user;
  $old_state = drupal_save_session();
  drupal_save_session(FALSE);
  $user = drupal_anonymous_user();

  services_set_server_info('original_user', $original_user);

  // Check authentication
  if (!isset($options['skip_authentication']) || !$options['skip_authentication']) {
    $endpoint_name = services_get_server_info('endpoint');
    $endpoint = services_endpoint_load($endpoint_name);

    foreach ($endpoint->authentication as $auth_module => $settings) {
      if (isset($settings) && $auth_error = services_auth_invoke($auth_module, 'authenticate_call', $settings, $controller, $args)) {
        return services_error($auth_error, 401);
      }
    }
  }

  // Load the proper file
  if (!empty($controller['file']) && $file = $controller['file']) {
    module_load_include($file['type'], $file['module'], (isset($file['name']) ? $file['name'] : NULL));
  }

  // Construct access arguments array
  if (isset($controller['access arguments'])) {
    $access_arguments = $controller['access arguments'];
    if (isset($controller['access arguments append']) && $controller['access arguments append']) {
      $access_arguments[] = $args;
    }
  }
  else {
    // Just use the arguments array if no access arguments have been specified
    $access_arguments = $args;
  }

  // Call default or custom access callback
  if (call_user_func_array($controller['access callback'], $access_arguments) != TRUE) {
    global $user;
    return services_error(t('Access denied for user @user', array(
      '@user' => isset($user->name) ? $user->name : 'anonymous',
    )), 401);
  }

  // Change working directory to drupal root to call drupal function,
  // then change it back to server module root to handle return.
  $server_root = getcwd();
  $drupal_path = services_get_server_info('drupal_path');
  if (isset($drupal_path) && $drupal_path) {
    chdir($drupal_path);
  }

  // Check if the arguments should be preprocessed
  if (!empty($controller['endpoint']['preprocess'])) {
    foreach ($controller['endpoint']['preprocess'] as $callable) {
      call_user_func_array($callable, array(&$args, $controller));
    }
  }

  // Execute the controller callback
  if ($server_info->debug) {
    watchdog('services', 'Called arguments: <pre>@arguments</pre>', array('@arguments' => print_r($args, TRUE)), WATCHDOG_DEBUG);
  }
  $result = call_user_func_array($controller['callback'], $args);

  // Check if the result should be post-processed
  if (!empty($controller['endpoint']['postprocess'])) {
    foreach ($controller['endpoint']['postprocess'] as $callable) {
      $result = call_user_func_array($callable, array($args, $controller, $result));
    }
  }
  
  if (drupal_save_session($user) === FALSE) {
    $user = $original_user;
    drupal_save_session($old_state);  
  }
  return $result;
}

/**
 * Gets information about a authentication module.
 *
 * @param string $module
 *  The module to get info for.
 * @return mixed
 *  The information array, or FALSE if the information wasn't found.
 */
function services_authentication_info($module) {
  $info = FALSE;
  if (!empty($module) && module_exists($module) && is_callable($module . '_services_authentication_info')) {
    $info = call_user_func($module . '_services_authentication_info');
  }
  return $info;
}

/**
 * Invokes a authentication module callback.
 *
 * @param string $module
 *  The authentication module to invoke the callback for.
 * @param string $method
 *  The callback to invoke.
 * @param string $arg1
 *  Optional. First argument to pass to the callback.
 * @param string $arg2
 *  Optional. Second argument to pass to the callback.
 * @param string $arg3
 *  Optional. Third argument to pass to the callback.
 * @return mixed
 *
 * Aren't these really the following?
 *  arg1 = Settings
 *  arg2 = Method
 *  arg3 = Controller
 *
 */
function services_auth_invoke($module, $method, &$arg1 = NULL, &$arg2 = NULL, &$arg3 = NULL) {
  // Get information about the auth module
  $info = services_authentication_info($module);
  $func = $info && !empty($info[$method]) ? $info[$method] : FALSE;
  if ($func) {
    if (!empty($info['file'])) {
      require_once(drupal_get_path('module', $module) . '/' . $info['file']);
    }

    if (is_callable($func)) {
      $args = func_get_args();
      // Replace module and method name and arg1 with reference to $arg1 and $arg2.
      array_splice($args, 0, 5, array(&$arg1, &$arg2, &$arg3));
      return call_user_func_array($func, $args);
    }
  }
  else {
    return TRUE;
  }
}

/**
 * Formats a resource uri using the formatter registered through
 * services_set_server_info().
 *
 * @param array $path
 *  An array of strings containing the component parts of the path to the resource.
 * @return string
 *  Returns the formatted resource uri, or NULL if no formatter has been registered.
 */
function services_resource_uri($path) {
  $endpoint_name = services_get_server_info('endpoint');
  $endpoint = services_endpoint_load($endpoint_name);
  if (isset($endpoint) && !empty($endpoint)) {
    if (is_array($endpoint->resources) && is_array($path) && isset($path[0]) && isset($endpoint->resources[$path[0]]['alias'])) {
        $path[0] = $endpoint->resources[$path[0]]['alias'];
    }
  }
  $formatter = services_get_server_info('resource_uri_formatter');
  if ($formatter) {
    return call_user_func($formatter, $path);
  }
  return NULL;
}

/**
 * Sets a server info value
 *
 * @param string $key
 *  The key of the server info value.
 * @param mixed $value
 *  The value.
 * @return void
 */
function services_set_server_info($key, $value) {
  $info = services_server_info_object();
  $info->$key = $value;
}

/**
 * Sets multiple server info values from a associative array.
 *
 * @param array $values
 *  An associative array containing server info values.
 * @return void
 */
function services_set_server_info_from_array($values) {
  $info = services_server_info_object();
  foreach ($values as $key => $value) {
    $info->$key = $value;
  }
}

/**
 * Gets a server info value.
 *
 * @param string $key
 *  The key for the server info value.
 * @param mixed $default
 *  The default value to return if the value isn't defined.
 * @return mixed
 *  The server info value.
 */
function services_get_server_info($key, $default = NULL) {
  $info = services_server_info_object();
  $value = $default;
  if (isset($info->$key)) {
    $value = $info->$key;
  }
  return $value;
}

/**
 * Gets the server info object.
 *
 * @param bool $reset
 *  Pass TRUE if the server info object should be reset.
 * @return object
 *  Returns the server info object.
 */
function services_server_info_object($reset = FALSE) {
  static $info;
  if (!$info) {
    $info = new stdClass();
  }
  return $info;
}

/**
 * Prepare an error message for returning to the server.
 *
 * @param string $message
 *  Error message.
 * @param int $code
 *  Optional. Error code. This often maps to the HTTP status codes. Defaults
 *  to 0.
 * @param mixed $data
 *  Optional. Information that can be used by the server to return information about the error. Defaults to null.
 * @return mixed
 */
function services_error($message, $code = 0, $data = NULL) {
  throw new ServicesException($message, $code, $data);
}

/**
 * Make any changes we might want to make to node.
 */
function services_node_load($node, $fields = array()) {
  if (!isset($node->nid)) {
    return NULL;
  }

  // Loop through and get only requested fields
  if (count($fields) > 0) {
    foreach ($fields as $field) {
      $val->{$field} = $node->{$field};
    }
  }
  else {
    $val = $node;
  }

  return $val;
}
