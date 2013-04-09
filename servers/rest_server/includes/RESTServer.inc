<?php

/**
 * @file
 * Class for handling REST calls.
 */

class RESTServer {
  protected $endpoint;
  protected $settings;
  protected $negotiator;
  protected $context;
  protected $controller;
  protected $resources;

  /**
   * Constructor. Initialize properties.
   */
  function __construct(ServicesContextInterface $context, ServicesContentTypeNegotiatorInterface $negotiator, $resources, $endpoint) {
    $this->context = $context;
    $this->negotiator = $negotiator;
    $this->resources = $resources;

    $this->endpoint = $endpoint;

    // Get the server settings from the endpoint.
    $this->settings = !empty($this->endpoint->server_settings) ? $this->endpoint->server_settings : array();
    // Normalize the settings so that we get the expected structure
    // and sensible defaults.
    $this->settings = rest_server_setup_settings($this->settings);
  }

  /**
   * Handles the call to the REST server
   */
  public function handle() {
    $controller = $this->getController();

    // Parse the request data
    $arguments = $this->getControllerArguments($controller);

    $formatter = $this->getResponseFormatter($controller);

    $this->modelAlterArguments($arguments, $formatter, $controller);
    services_set_server_info('resource_uri_formatter', array(&$this, 'uri_formatter'));

    try {
      $result = services_controller_execute($controller, $arguments);
    }
    catch (ServicesException $e) {
      $result = $this->handleException($e);
    }

    return $this->renderFormatterView($controller, $formatter, $result);
  }

  protected function getController() {
    if (empty($this->controller)) {
      $resource_name = $this->getResourceName();

      if (!empty($resource_name) && isset($this->resources[$resource_name])) {
        $resource = $this->resources[$resource_name];

        // Get the operation and fill with default values
        $this->controller = $this->resolveController($resource, $resource_name);
      }
      else {
        // This will stop the 404 from happening when you request just the endpoint.
        if ($this->endpoint->path == $resource_name) {
          $response = t('Services Endpoint "@name" has been setup successfully.', array('@name' => $this->endpoint->name));
          drupal_alter('services_endpoint_response', $response);
          return $response;
        }
        return services_error(t('Could not find resource @name.', array('@name' => $resource_name)), 404);
      }

      if (empty($this->controller)) {
        return services_error(t('Could not find the controller.'), 404);
      }
    }

    return $this->controller;
  }

  public function getCanonicalPath() {
    $canonical_path = &drupal_static('RESTServerGetCanonicalPath');
    if (empty($canonical_path)) {
      $canonical_path = $this->context->getCanonicalPath();
      $canonical_path = $this->negotiator->getParsedCanonicalPath($canonical_path);
    }
    return $canonical_path;
  }

  public function getCanonicalPathArray() {
    $canonical_path = $this->getCanonicalPath();
    $canonical_path_array = explode('/', $canonical_path);

    return $canonical_path_array;
  }

  public function getResourceName() {
    $canonical_path_array = $this->getCanonicalPathArray();
    $resource_name = array_shift($canonical_path_array);

    return $resource_name;
  }

  function getResponseFormatter($controller) {
    $mime_type = '';

    $canonical_path = $this->context->getCanonicalPath();
    $response_format = $this->negotiator->getResponseFormatFromURL($canonical_path);

    $formats = $this->responseFormatters();

    if (empty($response_format)) {
      $response_format = $this->getResponseFormatContentTypeNegotiations($mime_type, $canonical_path, $controller, $formats);
    }

    $formatter = array();


    if (isset($formats[$response_format])) {
      $formatter = $formats[$response_format];
    }

    // Check if we support the response format and determine the mime type
    if (empty($mime_type) && !empty($formatter) && (!isset($formatter['model']) || $this->supportedControllerModel($controller, $formatter))) {
      $mime_type = $formatter['mime types'][0];
    }

    if (empty($response_format) || empty($mime_type)) {
      return services_error(t('Unknown or unsupported response format.'), 406);
    }

    // Set the content type and render output.
    drupal_add_http_header('Content-type', $mime_type);

    return $formatter;
  }


  /**
   * Retrieve formatter from URL. If format is in the path, we remove it from $canonical_path.
   *
   * For example <endpoint>/<path>.<format>
   *
   * @param $canonical_path
   *
   * @return string
   */
  protected function getResponseFormatFromURL(&$canonical_path) {
    $matches = array();
    $response_format = '';
    if (preg_match('/^(.+)\.([^\.^\/]+)$/', $canonical_path, $matches)) {
      $canonical_path = $matches[1];
      $response_format = $matches[2];
    }

    return $response_format;
  }

  /**
   * Determine response format and mime type using headers to negotiate content types.
   */
  protected function getResponseFormatContentTypeNegotiations(&$mime_type, $canonical_path, $controller, $formats) {
    $response_format = $this->negotiator->getResponseFormatContentTypeNegotiations($mime_type, $canonical_path, $controller, $formats, $this->context);
    return $response_format;
  }

  /**
   * Determine the request method
   */
  protected function getRequestMethod() {
    return $this->context->getRequestMethod();
  }

  /**
   * Give the model (if any) a opportunity to alter the arguments.
   * This might be needed for the model to ensure that all the required
   * information is requested.
   *
   * @param $arguments
   */
  protected function modelAlterArguments(&$arguments, $formatter, $controller) {
    //
    if (isset($formatter['model'])) {
      $cm = &$controller['models'][$formatter['model']];
      if (!isset($cm['arguments'])) {
        $cm['arguments'] = array();
      }

      // Check if any of the model arguments have been overridden
      if (isset($cm['allow_overrides'])) {
        foreach ($cm['allow_overrides'] as $arg) {
          $get_formatter_model = $this->context->getGetVariable($formatter['model'] . ':' . $arg);
          if (!empty($get_formatter_model)) {
            $cm['arguments'][$arg] = $get_formatter_model;
          }
        }
      }

      if (isset($cm['class']) && class_exists($cm['class'])) {
        if (method_exists($cm['class'], 'alterArguments')) {
          call_user_func_array($cm['class'] . '::alterArguments', array(&$arguments, $cm['arguments']));
        }
      }
    }
  }

  /**
   * Formats a resource uri
   *
   * @param array $path
   *  An array of strings containing the component parts of the path to the resource.
   * @return string
   *  Returns the formatted resource uri
   */
  public function uri_formatter($path) {
    return url($this->context->getEndpointPath() . '/' . join($path, '/'), array(
      'absolute' => TRUE,
    ));
  }

  /**
   * Parses controller arguments from request
   *
   * @param array $controller
   *  The controller definition
   * @return void
   */
  protected function getControllerArguments($controller) {
    $path_array = $this->getCanonicalPathArray();
    array_shift($path_array);

    $data = $this->parseRequest($controller);
    drupal_alter('rest_server_request_parsed', $data, $controller);

    $headers = $this->parseRequestHeaders();
    drupal_alter('rest_server_headers_parsed', $headers);

    $sources = array(
      'path' => $path_array,
      'param' => $this->context->getGetVariable(),
      'data' => $data,
      'headers' => $headers,
    );
    // Map source data to arguments.
    return $this->getControllerArgumentsFromSources($controller, $sources);
  }

  /**
   * array $controller
   *   Controller definition
   * array $sources
   *   Array of sources for arguments. Consists of following elements:
   *  'path' - path requested
   *  'params' - GET variables
   *  'data' - parsed POST data
   *  'headers' - request headers
   *
   * @return array
   */
  protected function getControllerArgumentsFromSources($controller, $sources) {
    $arguments = array();
    if (!isset($controller['args'])) {
      return array();
    }

    foreach ($controller['args'] as $argument_number => $argument_info) {
      // Fill in argument from source
      if (isset($argument_info['source'])) {
        if (is_array($argument_info['source'])) {
          list($source) = array_keys($argument_info['source']);
          $key = $argument_info['source'][$source];
          if (isset($sources[$source][$key])) {
            $arguments[$argument_number] = $sources[$source][$key];
          }
        }
        else {
          if (isset($sources[$argument_info['source']])) {
            $arguments[$argument_number] = $sources[$argument_info['source']];
          }
        }
        // Convert to array if argument expected to be array.
        if (isset($argument_info['type']) && $argument_info['type'] == 'array' && isset($arguments[$argument_number])) {
          $arguments[$argument_number] = (array)$arguments[$argument_number];
        }
      }
      // When argument isn't set, insert default value if provided or
      // throw a exception if the argument isn't optional.
      if (!isset($arguments[$argument_number])) {
        if (!isset($argument_info['optional']) || !$argument_info['optional']) {
          return services_error(t('Missing required argument @arg', array('@arg' => $argument_info['name'])), 401);
        }
        // Set default value or NULL if default value is not set.
        $arguments[$argument_number] = isset($argument_info['default value']) ? $argument_info['default value'] : NULL;
      }
    }
    return $arguments;
  }

  protected function parseRequestHeaders() {
    $headers = array();
    $http_if_modified_since = $this->context->getServerVariable('HTTP_IF_MODIFIED_SINCE');
    if (!empty($http_if_modified_since)) {
      $headers['IF_MODIFIED_SINCE'] = strtotime(preg_replace('/;.*$/', '', $http_if_modified_since));
    }
    return $headers;
  }

  protected function parseRequest($controller) {
    $method = $this->getRequestMethod();
    switch ($method) {
      case 'POST':
      case 'PUT':
        $server_content_type = $this->context->getServerVariable('CONTENT_TYPE');
        if (!empty($server_content_type)) {
          $type = $this->parseContentHeader($server_content_type);
        }

        // Get the mime type for the request, default to form-urlencoded
        if (isset($type['value'])) {
          $mime = $type['value'];
        }
        else {
          $mime = 'application/x-www-form-urlencoded';
        }

        // Get the parser for the mime type
        $parser = $this->requestParser($mime, $controller);
        if (!$parser) {
          return services_error(t('Unsupported request content type @mime', array('@mime' => $mime)), 406);
        }

        $data = array();
        if (class_exists($parser) && in_array('ServicesParserInterface', class_implements($parser))) {
          $parser_object = new $parser;
          $data = $parser_object->parse($this->context);
        }
        return $data;

      default:
        return array();
    }
  }

  protected function parseContentHeader($value) {
    $ret_val = array();
    $value_pattern = '/^([^;]+)(;\s*(.+)\s*)?$/';
    $param_pattern = '/([a-z]+)=(([^\"][^;]+)|(\"(\\\"|[^"])+\"))/';
    $vm=array();

    if (preg_match($value_pattern, $value, $vm)) {
      $ret_val['value'] = $vm[1];
      if (count($vm)>2) {
        $pm = array();
        if (preg_match_all($param_pattern, $vm[3], $pm)) {
          $pcount = count($pm[0]);
          for ($i=0; $i<$pcount; $i++) {
            $value = $pm[2][$i];
            if (drupal_substr($value, 0, 1) == '"') {
              $value = stripcslashes(drupal_substr($value, 1, mb_strlen($value)-2));
            }
            $ret_val['param'][$pm[1][$i]] = $value;
          }
        }
      }
    }

    return $ret_val;
  }

  protected function renderFormatterView($controller, $formatter, $result) {
    // Wrap the results in a model class if required by the formatter
    if (isset($formatter['model'])) {
      $cm = $controller['models'][$formatter['model']];
      $model_arguments = isset($cm['arguments'])?$cm['arguments']:array();

      $model_class = new ReflectionClass($cm['class']);
      $result = $model_class->newInstanceArgs(array($result, $model_arguments));
    }

    $view_class = new ReflectionClass($formatter['view']);
    $view_arguments = isset($formatter['view arguments'])?$formatter['view arguments']:array();
    $view = $view_class->newInstanceArgs(array($result, $view_arguments));
    return $view->render();
  }

  /**
   * Get best match parser for $controller based on $mime type.
   */
  protected function requestParser($mime, $controller = NULL) {
    // Check if the controller has declared support for parsing the mime type.
    if ($controller && !empty($controller['rest request parsers'])) {
      $parser = $this->matchParser($mime, $controller['rest request parsers']);
      if ($parser) {
        return $parser;
      }
    }

    $parsers = rest_server_request_parsers();

    // Remove parsers that have been disabled for this endpoint.
    foreach (array_keys($parsers) as $key) {
      if (!$this->settings['parsers'][$key]) {
        unset($parsers[$key]);
      }
    }

    return $this->matchParser($mime, $parsers);
  }

  /**
   * Matches a mime-type against a set of parsers.
   *
   * @param string $mime
   *  The mime-type of the request.
   * @param array $parsers
   *  An associative array of parser callbacks keyed by mime-type.
   * @return mixed
   *  Returns a parser callback or FALSE if no match was found.
   */
  protected function matchParser($mime, $parsers) {
    $mimeparse = $this->negotiator->mimeParse();
    $mime_type = $mimeparse->best_match(array_keys($parsers), $mime);

    return ($mime_type) ? $parsers[$mime_type] : FALSE;
  }

  protected function responseFormatters($format = NULL) {
    $formatters = rest_server_response_formatters();

    // Remove formatters that have been disabled for this endpoint.
    foreach (array_keys($formatters) as $key) {
      if (!$this->settings['formatters'][$key]) {
        unset($formatters[$key]);
      }
    }

    if ($format) {
      return isset($formatters[$format]) ? $formatters[$format] : FALSE;
    }

    return $formatters;
  }

  protected function resolveController($resource, $resource_name) {
    $request_method = $this->getRequestMethod();

    $canonical_path_array = $this->getCanonicalPathArray();
    array_shift($canonical_path_array);

    $pc = count($canonical_path_array);
    $class = NULL;
    $operation = NULL;

    // We do not group "if" conditions on purpose for better readability.

    // 'index' method.
    if ($request_method == 'GET' && isset($resource['operations']['index']) && $this->checkNumberOfArguments($pc, $resource['operations']['index'])) {
      $class = 'operations';
      $operation = 'index';
    }

    // 'retrieve' method.
    // First path element should be not empty.
    if ($request_method == 'GET' && isset($resource['operations']['retrieve']) && $this->checkNumberOfArguments($pc, $resource['operations']['retrieve']) && !empty($canonical_path_array[0]) && $pc >= 1) {
      $class = 'operations';
      $operation = 'retrieve';
    }

    // 'relationships'
    // First path element should be not empty,
    // second should be name of targeted action.
    if ($request_method == 'GET' && $pc >= 2 && isset($resource['relationships'][$canonical_path_array[1]])
         && $this->checkNumberOfArguments($pc, $resource['relationships'][$canonical_path_array[1]], 1) && isset($canonical_path_array[0])) {
      $class = 'relationships';
      $operation = $canonical_path_array[1];
    }

    // 'update'
    // First path element should be not empty.
    if ($request_method == 'PUT' && isset($resource['operations']['update']) && $this->checkNumberOfArguments($pc, $resource['operations']['update']) && !empty($canonical_path_array[0]) && $pc >= 1) {
      $class = 'operations';
      $operation = 'update';
    }

    // 'delete'
    // First path element should be not empty.
    if ($request_method == 'DELETE' && isset($resource['operations']['delete']) && $this->checkNumberOfArguments($pc, $resource['operations']['delete']) && !empty($canonical_path_array[0]) && $pc >= 1) {
      $class = 'operations';
      $operation = 'delete';
    }

    // 'create' method.
    // First path element should be not empty.
    if ($request_method == 'POST' && isset($resource['operations']['create']) && $this->checkNumberOfArguments($pc, $resource['operations']['create'])) {
      $class = 'operations';
      $operation = 'create';
    }

    // 'actions'
    // Forst path element should be action name
    if ($request_method == 'POST' && $pc >= 1 && isset($resource['actions'][$canonical_path_array[0]]) && $this->checkNumberOfArguments($pc, $resource['actions'][$canonical_path_array[0]], 1)) {
      $class = 'actions';
      $operation = $canonical_path_array[0];
    }

    // 'targeted_actions'
    // First path element should be not empty,
    // second should be name of targeted action.
    if ($request_method == 'POST' && $pc >= 2 && isset($resource['targeted_actions'][$canonical_path_array[1]])
         && $this->checkNumberOfArguments($pc, $resource['targeted_actions'][$canonical_path_array[1]], 1) && !empty($canonical_path_array[0])) {
      $class = 'targeted_actions';
      $operation = $canonical_path_array[1];
    }

    $controller = FALSE;
    if (!empty($class) && !empty($operation) && !empty($resource[$class][$operation])) {
      $controller = $resource[$class][$operation];
      if (isset($resource['file']) && empty($controller['file'])) {
        $controller['file'] = $resource['file'];
      }
    }
    services_request_apply_version($controller, array('method' => $operation, 'resource' => $resource_name));
    return $controller;
  }

  /**
   * Count possible numbers of 'path' arguments of the method.
   */
  private function checkNumberOfArguments($args_number, $resource_operation, $required_args = 0) {
    $not_required_args = 0;

    if (isset($resource_operation['args'])) {
      foreach ($resource_operation['args'] as $argument) {
        if (isset($argument['source']) && is_array($argument['source']) && isset($argument['source']['path'])) {
          if (!empty($argument['optional'])) {
            $not_required_args++;
          }
          else {
            $required_args++;
          }
        }
      }
    }

    return $args_number >= $required_args && $args_number <= $required_args + $not_required_args;
  }

  function handleException($exception){
    $error_code = $exception->getCode();
    $error_message = $exception->getMessage();
    $error_data = method_exists($exception, 'getData') ? $exception->getData() : '';

    switch ($error_code) {
      case 204:
        $error_header_status_message = '204 No Content: ' . $error_message;
        break;
      case 304:
        $error_header_status_message =  '304 Not Modified: ' . $error_message;
        break;
      case 401:
        $error_header_status_message =  '401 Unauthorized: ' . $error_message;
        break;
      case 404:
        $error_header_status_message =  '404 Not found: ' . $error_message;
        break;
      case 406:
        $error_header_status_message = '406 Not Acceptable: ' . $error_message;
        break;
      default:
        if ($error_code >= 400 && $error_code < 600) {
          $error_header_status_message = $error_code . ':' . $error_message;
        }
        else {
          $error_header_status_message = '500 Internal Server Error: An error occurred (' . $error_code . '): ' . $error_message;
        }
        break;
    }

    $error_alter_array = array(
      'code' => $error_code,
      'header_message' => &$error_header_status_message,
      'body_data' => &$error_data,
    );
    drupal_alter('rest_server_execute_errors', $error_alter_array, $controller, $arguments);

    drupal_add_http_header('Status', $error_header_status_message);

    return $error_data;
  }
}
